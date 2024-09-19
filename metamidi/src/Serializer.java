import java.util.*;

public class Serializer {

    private static final boolean PRE_PIANO_BIT = false;
    private static final boolean PRE_NOTE_BIT = true;
    public static final int PIANO_TYPE_BIT_COUNT = 3;

    public byte[] serialize(final Map<FNEH_Piano, Set<Integer>> activeNotes) {

        int totalSizeNeeded = activeNotes.entrySet().stream().map(entry -> bitsRequiredToCode(entry.getKey(), entry.getValue()))
                .reduce(Integer::sum).orElse(0);

        BitSet bitSet = new BitSet(totalSizeNeeded);
        int sizeSoFar = totalSizeNeeded - 1;

        for (Map.Entry<FNEH_Piano, Set<Integer>> entry : activeNotes.entrySet()) {
            sizeSoFar = serialize(entry.getKey(), entry.getValue(), bitSet, sizeSoFar);
        }

        return bitSet.toByteArray();

    }

    private int serialize(FNEH_Piano fnehPiano, Set<Integer> activeNotes, BitSet bitSet, int sizeSoFar) {

        bitSet.set(sizeSoFar, PRE_PIANO_BIT);
        sizeSoFar --;
        setBitsAt(bitSet, sizeSoFar, encodePiano(fnehPiano), PIANO_TYPE_BIT_COUNT);
        sizeSoFar -= PIANO_TYPE_BIT_COUNT;

        for (int note : activeNotes) {
            sizeSoFar = addNoteToBitSet(bitSet, sizeSoFar, note, fnehPiano);
        }
        return sizeSoFar;
    }

    private int bitsRequiredToCode(FNEH_Piano piano, Set<Integer> activeNotes) {
        return 1 // bit required to encode piano or key
                + 3 //bits requires to encode the piano itself
                + activeNotes.size()
                * (1         //bit required to encode it's a key
                + numberOfBitsPerNotOf(piano));
    }

    private int addNoteToBitSet(BitSet bitSet, int sizeSoFar, int activeNote, FNEH_Piano fnehPiano) {
        bitSet.set(sizeSoFar, PRE_NOTE_BIT);
        setBitsAt(bitSet, sizeSoFar - 1, activeNote, numberOfBitsPerNotOf(fnehPiano));
        return sizeSoFar - 1 - numberOfBitsPerNotOf(fnehPiano);
    }

    private int encodePiano(FNEH_Piano piano) {
        return switch (piano) {
            case FNEH_Piano.FNEH_88_KEY -> 1;
            case FNEH_63_KEY -> 2;
            case FNEH_49_KEY -> 3;
            case N_OVATION_25_KEY -> 4;
            case MPK_MINI_25_KEY -> 5;
        };
    }

    private int numberOfBitsPerNotOf(FNEH_Piano piano) {
        return switch (piano) {
            case FNEH_Piano.FNEH_88_KEY -> 7;
            case FNEH_63_KEY, FNEH_49_KEY -> 6;
            case N_OVATION_25_KEY, MPK_MINI_25_KEY -> 5;
        };
    }

    private void setBitsAt(BitSet bitset, int sizeSoFar, int number, int bitCount) {
        if (number >= 1 << bitCount) {
            throw new IndexOutOfBoundsException("Can only cover up to 7 pianos. " +
                    "This is just a limit in the encoding and can be increased. " +
                    "Please reach out to me to help.");
        }
        int indexer = 1 << (bitCount - 1);
        for (int i = 0; i < bitCount; i++) {
            if ((number & indexer) != 0) {
                bitset.set(sizeSoFar - i);
            }
            indexer = indexer >> 1;
        }
    }


}
