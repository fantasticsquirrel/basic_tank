import { StatusBar } from 'expo-status-bar';
import React, { useCallback, useEffect, useRef, useState } from 'react';
import { ActivityIndicator, GestureResponderEvent, SafeAreaView, StyleSheet, Text, TextInput, TouchableOpacity, View } from 'react-native';

type Mode = 'auto' | 'manual';
type Status = { mode: Mode; distanceCm: number | null; rangeValid: boolean; motion: string; phase: string; commandFresh: boolean; lastSequence: number; ip: string };
const clamp = (value: number, low: number, high: number) => Math.max(low, Math.min(high, value));

function TrackControl({ label, value, onChange }: { label: string; value: number; onChange: (value: number) => void }) {
  const update = (event: GestureResponderEvent) => {
    const normalized = 1 - (event.nativeEvent.locationY / 244) * 2;
    onChange(Math.round(clamp(normalized, -1, 1) * 255));
  };
  const position = `${50 - (value / 255) * 43}%` as `${number}%`;
  return <View style={styles.trackColumn}>
    <Text style={styles.trackLabel}>{label}</Text>
    <View testID={`track-${label.toLowerCase()}`} style={styles.track}
      onStartShouldSetResponder={() => true} onMoveShouldSetResponder={() => true}
      onResponderGrant={update} onResponderMove={update}
      onResponderRelease={() => onChange(0)} onResponderTerminate={() => onChange(0)}>
      <Text style={styles.directionTop}>FWD</Text><View style={styles.centerLine} /><Text style={styles.directionBottom}>REV</Text>
      <View style={[styles.trackThumb, { top: position }]}><View style={styles.thumbGrip} /><View style={styles.thumbGrip} /><View style={styles.thumbGrip} /></View>
    </View>
    <Text style={styles.trackValue}>{value > 0 ? '+' : ''}{value}</Text>
  </View>;
}

export default function App() {
  const [host, setHost] = useState('192.168.1.50');
  const [token, setToken] = useState('');
  const [connected, setConnected] = useState(false);
  const [connecting, setConnecting] = useState(false);
  const [mode, setModeState] = useState<Mode>('auto');
  const [left, setLeft] = useState(0); const [right, setRight] = useState(0);
  const [status, setStatus] = useState<Status | null>(null); const [error, setError] = useState('');
  const sequence = useRef(0); const controls = useRef({ left: 0, right: 0 });
  useEffect(() => { controls.current = { left, right }; }, [left, right]);
  const baseUrl = `http://${host.trim().replace(/^https?:\/\//, '').replace(/\/$/, '')}`;
  const request = useCallback(async (path: string, options: RequestInit = {}) => {
    const response = await fetch(`${baseUrl}${path}`, { ...options, headers: { 'X-Control-Token': token.trim(), ...(options.headers || {}) } });
    if (!response.ok) throw new Error((await response.json().catch(() => ({}))).error || `ESP32 returned ${response.status}`);
    return response.json();
  }, [baseUrl, token]);

  const connect = async () => {
    setConnecting(true); setError('');
    try { const next = await request('/api/status') as Status; sequence.current = next.lastSequence; setStatus(next); setModeState(next.mode); setConnected(true); }
    catch (reason) { setConnected(false); setError(reason instanceof Error ? reason.message : 'Could not reach tank'); }
    finally { setConnecting(false); }
  };
  const setMode = async (next: Mode) => {
    if (!connected || next === mode) return;
    setLeft(0); setRight(0); setError('');
    try { await request(`/api/mode?value=${next}`, { method: 'POST' }); setModeState(next); }
    catch (reason) { setError(reason instanceof Error ? reason.message : 'Mode change failed'); }
  };
  useEffect(() => {
    if (!connected) return;
    const timer = setInterval(async () => {
      try { const next = await request('/api/status') as Status; sequence.current = Math.max(sequence.current, next.lastSequence); setStatus(next); setModeState(next.mode); setError(''); }
      catch { setConnected(false); setError('Connection lost — motors will stop'); }
    }, 1000);
    return () => clearInterval(timer);
  }, [connected, request]);
  useEffect(() => {
    if (!connected || mode !== 'manual') return;
    const send = async () => {
      const current = controls.current; const seq = ++sequence.current;
      try { await request(`/api/control?left=${current.left}&right=${current.right}&seq=${seq}`, { method: 'POST' }); }
      catch { setConnected(false); setError('Control link lost — dead-man stop engaged'); }
    };
    send(); const timer = setInterval(send, 140); return () => clearInterval(timer);
  }, [connected, mode, request]);

  return <SafeAreaView style={styles.safe}><StatusBar style="light" /><View style={styles.shell}>
    <View style={styles.header}><View><Text style={styles.eyebrow}>BASIC TANK // ESP32</Text><Text style={styles.title}>TRACK COMMAND</Text></View><View style={[styles.statusLamp, connected && styles.statusLampOnline]} /></View>
    <View testID="connection-card" style={styles.connectionCard}>
      <View style={styles.inputRow}><View style={styles.inputGroup}><Text style={styles.inputLabel}>TANK IP</Text><TextInput value={host} onChangeText={setHost} autoCapitalize="none" style={styles.input} /></View><View style={[styles.inputGroup, styles.tokenGroup]}><Text style={styles.inputLabel}>CONTROL TOKEN</Text><TextInput value={token} onChangeText={setToken} autoCapitalize="none" secureTextEntry style={styles.input} placeholder="required" placeholderTextColor="#59616b" /></View></View>
      <TouchableOpacity style={[styles.connectButton, connected && styles.connectedButton]} onPress={connect} disabled={connecting}>{connecting ? <ActivityIndicator color="#08110d" /> : <Text style={styles.connectText}>{connected ? 'RECONNECT' : 'CONNECT DIRECT'}</Text>}</TouchableOpacity>
      <Text style={[styles.linkText, error ? styles.errorText : null]}>{error || (connected ? `LINKED // ${status?.ip || host}` : 'LOCAL WI-FI // NO CLOUD RELAY')}</Text>
    </View>
    <View testID="mode-bar" style={styles.modeBar}>{(['auto', 'manual'] as Mode[]).map(item => <TouchableOpacity key={item} style={[styles.modeButton, mode === item && styles.modeActive]} onPress={() => setMode(item)} disabled={!connected}><Text style={[styles.modeText, mode === item && styles.modeTextActive]}>{item.toUpperCase()}</Text></TouchableOpacity>)}</View>
    <View style={styles.telemetry}><View><Text style={styles.telemetryLabel}>RANGE</Text><Text style={styles.telemetryValue}>{status?.rangeValid ? `${status.distanceCm?.toFixed(1)} CM` : '—'}</Text></View><View><Text style={styles.telemetryLabel}>MOTION</Text><Text style={styles.telemetryValue}>{status?.motion?.toUpperCase() || 'OFFLINE'}</Text></View><View><Text style={styles.telemetryLabel}>STATE</Text><Text style={styles.telemetryValue}>{status?.phase?.toUpperCase() || '—'}</Text></View></View>
    {mode === 'manual' ? <View style={[styles.controls, !connected && styles.disabled]}><TrackControl label="LEFT" value={left} onChange={setLeft} /><View style={styles.controlCenter}><Text style={styles.controlHint}>HOLD + DRAG</Text><TouchableOpacity style={styles.stopButton} onPress={() => { setLeft(0); setRight(0); }}><Text style={styles.stopText}>STOP</Text></TouchableOpacity><Text style={styles.controlHint}>RELEASE = 0</Text></View><TrackControl label="RIGHT" value={right} onChange={setRight} /></View>
      : <View style={styles.autoPanel}><Text style={styles.autoGlyph}>◎</Text><Text style={styles.autoTitle}>AUTONOMOUS NAVIGATION</Text><Text style={styles.autoCopy}>Ultrasonic avoidance is active. Switch to manual to take independent control of both tracks.</Text></View>}
    <Text style={styles.footer}>DEAD-MAN LINK // 450 MS FAIL-SAFE</Text>
  </View></SafeAreaView>;
}

const c = { bg: '#080b0d', panel: '#11161a', edge: '#293139', text: '#edf2ee', muted: '#84909a', green: '#76f7a7', amber: '#f4b84a', red: '#ff5d5d' };
const styles = StyleSheet.create({
  safe:{flex:1,backgroundColor:c.bg},shell:{flex:1,width:'100%',maxWidth:520,alignSelf:'center',paddingHorizontal:18,paddingTop:20,paddingBottom:14},header:{flexDirection:'row',justifyContent:'space-between',alignItems:'center',marginBottom:16},eyebrow:{color:c.green,fontSize:11,fontWeight:'800',letterSpacing:2.2},title:{color:c.text,fontSize:27,fontWeight:'900',letterSpacing:1.2,marginTop:2},statusLamp:{width:14,height:14,borderRadius:7,backgroundColor:'#4b2424',borderWidth:2,borderColor:'#743030'},statusLampOnline:{backgroundColor:c.green,borderColor:'#c2ffd7'},
  connectionCard:{backgroundColor:c.panel,borderWidth:1,borderColor:c.edge,borderRadius:16,padding:13},inputRow:{flexDirection:'row',gap:9},inputGroup:{flex:1},tokenGroup:{flex:1.05},inputLabel:{color:c.muted,fontSize:9,fontWeight:'800',letterSpacing:1.3,marginBottom:5},input:{backgroundColor:'#090d0f',color:c.text,borderWidth:1,borderColor:'#303940',borderRadius:8,paddingHorizontal:10,height:42,fontSize:13},connectButton:{marginTop:10,backgroundColor:c.green,borderRadius:9,height:42,justifyContent:'center',alignItems:'center'},connectedButton:{backgroundColor:'#b7c2ba'},connectText:{color:'#08110d',fontWeight:'900',letterSpacing:1.4,fontSize:12},linkText:{color:c.green,textAlign:'center',marginTop:8,fontSize:9,fontWeight:'700',letterSpacing:1},errorText:{color:c.red},
  modeBar:{flexDirection:'row',backgroundColor:'#0d1114',borderRadius:12,padding:4,marginTop:13,borderWidth:1,borderColor:c.edge},modeButton:{flex:1,paddingVertical:10,borderRadius:8,alignItems:'center'},modeActive:{backgroundColor:c.amber},modeText:{color:c.muted,fontWeight:'900',letterSpacing:2},modeTextActive:{color:'#181106'},telemetry:{flexDirection:'row',justifyContent:'space-between',paddingVertical:13,paddingHorizontal:5,borderBottomWidth:1,borderBottomColor:c.edge},telemetryLabel:{color:c.muted,fontSize:8,letterSpacing:1.4,fontWeight:'800'},telemetryValue:{color:c.text,fontSize:12,fontWeight:'800',marginTop:3},
  controls:{flex:1,minHeight:320,flexDirection:'row',justifyContent:'space-between',alignItems:'center',paddingTop:12},disabled:{opacity:.42},trackColumn:{alignItems:'center'},trackLabel:{color:c.text,fontSize:11,fontWeight:'900',letterSpacing:2,marginBottom:7},track:{width:94,height:244,backgroundColor:'#0b0f12',borderRadius:18,borderWidth:2,borderColor:'#313b43',overflow:'hidden',position:'relative'},directionTop:{position:'absolute',top:12,alignSelf:'center',color:'#587565',fontSize:8,fontWeight:'900',letterSpacing:1.4},directionBottom:{position:'absolute',bottom:12,alignSelf:'center',color:'#765c55',fontSize:8,fontWeight:'900',letterSpacing:1.4},centerLine:{position:'absolute',top:'50%',left:8,right:8,height:1,backgroundColor:'#4b555d'},trackThumb:{position:'absolute',left:9,right:9,height:34,marginTop:-17,backgroundColor:c.amber,borderRadius:8,justifyContent:'center',alignItems:'center',gap:3},thumbGrip:{width:35,height:2,backgroundColor:'#6b4b0c',opacity:.7},trackValue:{color:c.green,marginTop:7,fontVariant:['tabular-nums'],fontWeight:'800'},controlCenter:{alignItems:'center',gap:14},controlHint:{color:c.muted,fontSize:8,fontWeight:'800',letterSpacing:1.2,maxWidth:66,textAlign:'center'},stopButton:{width:68,height:68,borderRadius:34,backgroundColor:'#3a1114',borderWidth:3,borderColor:c.red,justifyContent:'center',alignItems:'center'},stopText:{color:'#fff',fontWeight:'900',fontSize:13,letterSpacing:1},
  autoPanel:{flex:1,minHeight:300,justifyContent:'center',alignItems:'center',paddingHorizontal:28},autoGlyph:{color:c.green,fontSize:80,lineHeight:88},autoTitle:{color:c.text,fontSize:17,fontWeight:'900',letterSpacing:1.4,marginTop:4},autoCopy:{color:c.muted,textAlign:'center',lineHeight:20,marginTop:10,fontSize:13},footer:{color:'#53606a',textAlign:'center',fontSize:8,fontWeight:'800',letterSpacing:1.7,marginTop:6}
});
