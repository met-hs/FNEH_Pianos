import midi.MidiDeviceFinder;

import javax.sound.midi.*;
import java.io.IOException;

public class FNEHChannelForwarder {

    private static final String DEVICE = "UM-ONE";

    public static void main(String[] args) throws MidiUnavailableException, IOException, InvalidMidiDataException, InterruptedException {

        MidiDevice arduino = MidiDeviceFinder.findReceivingDevice("Arduino Due");
        assert arduino != null;
        arduino.open();

        MidiChannelListener midiChannelListener = new MidiChannelListener("UM-ONE", arduino.getReceiver());
        midiChannelListener.start();

    }


}
