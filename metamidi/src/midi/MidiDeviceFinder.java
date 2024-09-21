package midi;

import javax.sound.midi.MidiDevice;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.MidiUnavailableException;

public class MidiDeviceFinder {

    public static MidiDevice findTransmittingByName(String deviceName) throws MidiUnavailableException {

        MidiDevice.Info[] infos = MidiSystem.getMidiDeviceInfo();

        for (MidiDevice.Info info : infos) {
            MidiDevice device = MidiSystem.getMidiDevice(info);
            printDeviceInfo(info, device);
            if (info.getName().equals(deviceName)) {
                if (device.getMaxTransmitters() != 0) {
                    return device;
                }
            }
        }
        return null;
    }

    private static void printDeviceInfo(MidiDevice.Info info, MidiDevice device) {
        System.out.println("Device: " + info.getName());
        System.out.println("    Description: " + info.getDescription());
        System.out.println("    Vendor: " + info.getVendor());
        System.out.println("    Version: " + info.getVersion());
        System.out.println("    Max Transmitters: " + device.getMaxTransmitters());
        System.out.println("    Max Receivers: " + device.getMaxReceivers());
        System.out.println("    Receivers: " + device.getReceivers());
        System.out.println("    Transmitters: " + device.getTransmitters());
        System.out.println();
    }

    public static MidiDevice findReceivingDevice(String deviceName) throws MidiUnavailableException {

        MidiDevice.Info[] infos = MidiSystem.getMidiDeviceInfo();

        for (MidiDevice.Info info : infos) {
            MidiDevice device = MidiSystem.getMidiDevice(info);
            printDeviceInfo(info, device);
            if (info.getName().equals(deviceName)) {
                if (device.getMaxReceivers() != 0) {
                    System.out.println("FOUND THE DEVICE " + deviceName);
                    return device;
                }
            }
        }
        return null;
    }

}
