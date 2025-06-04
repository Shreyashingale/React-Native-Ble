// BluetoothService.ts
import { BleManager, Device, Characteristic } from 'react-native-ble-plx';
import { PermissionsAndroid, Platform } from 'react-native';

class BluetoothService {
  manager = new BleManager();
  connectedDevice: Device | null = null;

  async requestPermissions() {
    if (Platform.OS === 'android') {
      await PermissionsAndroid.requestMultiple([
        PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
        PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN,
        PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT,
      ]);
    }
  }

  scanDevices(onDeviceFound: (device: Device) => void) {
    this.manager.startDeviceScan(null, null, (error, device) => {
      if (error) {
        console.error('Scan error:', error);
        return;
      }

      if (device?.name) {
        onDeviceFound(device);
      }
    });
  }

  stopScan() {
    this.manager.stopDeviceScan();
  }

  async connectToDevice(deviceId: string): Promise<Device | null> {
    try {
      const device = await this.manager.connectToDevice(deviceId);
      await device.discoverAllServicesAndCharacteristics();
      this.connectedDevice = device;
      return device;
    } catch (err) {
      console.error('Connection error:', err);
      return null;
    }
  }

  async readCharacteristic(
    serviceUUID: string,
    characteristicUUID: string
  ): Promise<string | null> {
    if (!this.connectedDevice) return null;

    try {
      const char = await this.connectedDevice.readCharacteristicForService(
        serviceUUID,
        characteristicUUID
      );
      return char?.value || null;
    } catch (err) {
      console.error('Read error:', err);
      return null;
    }
  }

  async writeCharacteristic(
    serviceUUID: string,
    characteristicUUID: string,
    base64Value: string
  ): Promise<void> {
    if (!this.connectedDevice) return;

    try {
      await this.connectedDevice.writeCharacteristicWithResponseForService(
        serviceUUID,
        characteristicUUID,
        base64Value
      );
    } catch (err) {
      console.error('Write error:', err);
    }
  }

  disconnect() {
    if (this.connectedDevice) {
      this.connectedDevice.cancelConnection();
      this.connectedDevice = null;
    }
  }
}

export const bleService = new BluetoothService();
