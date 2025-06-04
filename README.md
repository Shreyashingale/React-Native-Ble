# React Native BLE Project

A React Native application with Bluetooth Low Energy (BLE) functionality built using Expo and TypeScript.

## Quick Start

If you've cloned this repository, you can run it directly:

```bash
npm install
npx expo run:android
```

## Setup from Scratch

Follow these steps if you want to create the project from the beginning:

### 1. Create React Native Project

```bash
npx create-expo-app -t expo-template-blank-typescript [project_name]
```

### 2. Install Expo CLI

```bash
npm install -g expo
```

### 3. Configure Expo Version

**Important**: You must use Expo version 49 or lower due to dependency conflicts with the Bluetooth package.

1. Check your `package.json` file and verify the Expo version is compatible with the BLE package
2. If you need to change the version:
   - Update the version in `package.json`
   - Delete `node_modules` and `package-lock.json`
   - Run `npm install` again

### 4. Install BLE Dependencies

```bash
npx expo install react-native-ble-plx @config-plugins/react-native-ble-plx
npx expo install expo-device react-native-base64
```

### 5. Configuration Files

Ensure you have the following configuration files:

- Copy or create the eas.json file if it doesn't exist, using the same content from the repository file
- Apply the necessary changes mentioned in the app.json file for BLE, or copy the app.json from the repository file

### 6. Prebuild and Final Setup

```bash
npx expo prebuild
npx expo install expo-dev-client
```

### 7. Run the Application

```bash
npx expo run:android
```

## Important Notes

- Cross-verify which version of Expo the Bluetooth package supports before installation
- Always remove `node_modules` and `package-lock.json` when changing Expo versions
- Ensure all configuration files are properly set up before running prebuild

## Resources

- [YouTube Tutorial](https://www.youtube.com/watch?v=UuHLPsjp6fM&t=159s)
- [BLE with React Native and Arduino Guide](https://www.cjoshmartin.com/blog/bluetooth-low-energy-ble-with-react-native-and-arduino)

## Troubleshooting

If you encounter dependency conflicts:

1. Check the Expo version compatibility with the BLE package
2. Clear your cache: `npm cache clean --force`
3. Delete `node_modules` and `package-lock.json`
4. Reinstall dependencies: `npm install`

## Requirements

- Node.js
- Expo CLI
- Android development environment
- Physical Android device (BLE functionality requires hardware)

---

*This project uses Bluetooth Low Energy functionality which requires testing on physical devices rather than emulators.*