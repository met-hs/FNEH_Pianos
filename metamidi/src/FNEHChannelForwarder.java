import midi.MidiDeviceFinder;

import javax.sound.midi.*;
import java.io.IOException;

public class FNEHChannelForwarder {

//    private static final String DEVICE = "UM-ONE";
    private static final String DEVICE = "Bass Station II";


    public static void main(String[] args) throws MidiUnavailableException, IOException, InvalidMidiDataException, InterruptedException {

        MidiDevice arduino = MidiDeviceFinder.findReceivingDevice("Arduino Due");
        assert arduino != null;
        arduino.open();

        MidiChannelListener midiChannelListener0 = new MidiChannelListener(DEVICE, arduino.getReceiver(), 0);
        MidiChannelListener midiChannelListener1 = new MidiChannelListener(DEVICE, arduino.getReceiver(), 1);
        MidiChannelListener midiChannelListener2 = new MidiChannelListener(DEVICE, arduino.getReceiver(), 2);
        MidiChannelListener midiChannelListener3 = new MidiChannelListener(DEVICE, arduino.getReceiver(), 3);
        MidiChannelListener midiChannelListener4 = new MidiChannelListener(DEVICE, arduino.getReceiver(), 4);
        midiChannelListener0.start();
        midiChannelListener1.start();
        midiChannelListener2.start();
        midiChannelListener3.start();
        midiChannelListener4.start();
    }


}
