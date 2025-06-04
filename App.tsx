// App.tsx
import React, { useEffect, useState } from 'react';
import {
  View,
  Text,
  Button,
  FlatList,
  TouchableOpacity,
  TextInput,
  StyleSheet,
} from 'react-native';
import { Device, BleManager } from 'react-native-ble-plx';
import base64 from 'react-native-base64';
import { bleService } from './services/BluetoothService';

const SERVICE_UUID = '6e400001-b5a3-f393-e0a9-e50e24dcca9e';
const WRITE_CHAR_UUID = '6e400002-b5a3-f393-e0a9-e50e24dcca9e';
const READ_CHAR_UUID = '6e400003-b5a3-f393-e0a9-e50e24dcca9e';

export default function App() {
  const [devices, setDevices] = useState<Device[]>([]);
  const [connectedDevice, setConnectedDevice] = useState<Device | null>(null);
  const [inputText, setInputText] = useState('');
  const [receivedText, setReceivedText] = useState('');

  useEffect(() => {
    bleService.requestPermissions();
    return () => bleService.disconnect();
  }, []);

  const startScan = () => {
    setDevices([]);
    bleService.scanDevices((device) => {
      setDevices((prev) => {
        if (!prev.find((d) => d.id === device.id)) return [...prev, device];
        return prev;
      });
    });

    setTimeout(() => bleService.stopScan(), 10000);
  };

  const connect = async (deviceId: string) => {
    const device = await bleService.connectToDevice(deviceId);
    if (device) {
      setConnectedDevice(device);
      alert(`Connected to ${device.name}`);

      // Set up notifications
      device.monitorCharacteristicForService(
        SERVICE_UUID,
        READ_CHAR_UUID,
        (error, characteristic) => {
          if (error) {
            console.error('Notification error:', error);
            return;
          }
          const decoded = base64.decode(characteristic?.value || '');
          setReceivedText((prev) => prev + '\n' + decoded);
        }
      );
    }
  };

  const writeData = async () => {
    if (connectedDevice && inputText) {
      const encoded = base64.encode(inputText);
      await bleService.writeCharacteristic(
        SERVICE_UUID,
        WRITE_CHAR_UUID,
        encoded
      );
      setInputText('');
    }
  };

  return (
    <View style={{ padding: 20, marginTop: 40 }}>
      <Button title="Scan BLE Devices" onPress={startScan} />
      <FlatList
        data={devices}
        keyExtractor={(item) => item.id}
        renderItem={({ item }) => (
          <TouchableOpacity
            style={styles.deviceItem}
            onPress={() => connect(item.id)}
          >
            <Text>{item.name || 'Unnamed Device'}</Text>
            <Text style={styles.deviceId}>{item.id}</Text>
          </TouchableOpacity>
        )}
      />
      {connectedDevice && (
        <>
          <Text style={{ marginVertical: 10 }}>
            Connected to: {connectedDevice.name}
          </Text>

          <TextInput
            placeholder="Type to send..."
            value={inputText}
            onChangeText={setInputText}
            style={styles.input}
          />
          <Button title="Send to ESP32" onPress={writeData} />

          <Text style={{ marginTop: 20, fontWeight: 'bold' }}>Received:</Text>
          <Text>{receivedText || '(No data yet)'}</Text>
        </>
      )}
    </View>
  );
}

const styles = StyleSheet.create({
  deviceItem: {
    padding: 10,
    borderBottomWidth: 1,
    borderColor: '#ccc',
  },
  deviceId: {
    fontSize: 12,
    color: 'gray',
  },
  input: {
    borderWidth: 1,
    borderColor: '#ccc',
    padding: 8,
    marginVertical: 10,
  },
});
