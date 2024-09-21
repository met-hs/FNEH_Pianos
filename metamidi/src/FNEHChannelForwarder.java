import midi.MidiDeviceFinder;

import javax.sound.midi.MidiDevice;
import javax.sound.midi.MidiUnavailableException;
import java.io.IOException;

public class FNEHChannelForwarder {

    private static final String DEVICE = "UM-ONE";

    public static void main(String[] args) throws MidiUnavailableException, IOException {

        MidiDevice arduino = MidiDeviceFinder.findReceivingDevice("Arduino Due");
        assert arduino != null;
        MidiChannelListener midiChannelListener = new MidiChannelListener(DEVICE, arduino.getReceiver());
        midiChannelListener.start();

        // Keep the application running to receive messages
        System.out.println("Listening for MIDI messages. Press Enter to exit.");
        System.in.read();

        // Close the MIDI device
        arduino.close();
        midiChannelListener.close();
    }


}
