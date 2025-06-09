import java.util.Scanner;

public class ColourManager {
    public static final String RESET = "\u001B[0m";
    public static final String RED = "\u001B[31m";
    public static final String ORANGE = "\u001B[38;5;208m"; // ANSI 256-color orange
    public static final String YELLOW = "\u001B[33m";
    public static final String GREEN = "\u001B[32m";
    public static final String BLUE = "\u001B[34m";
    public static final String INDIGO = "\u001B[38;5;54m";   // Close to indigo
    public static final String VIOLET = "\u001B[35m";        // Purple as violet
    public static final String CYAN = "\u001B[36m";
    public static final String WHITE = "\u001B[37m";
    public static String currentOutputColour = ORANGE;
    public static String currentInputColour = BLUE;
    public static String currentOptionColour = WHITE;
    public static String currentSuccessColour = GREEN;
    public static String currentErrorColour = RED;

    // Colour.ouColour()
    public static String ouColour() {
        return currentOutputColour;
    }

    // Colour.inColour()
    public static String inColour() {
        return currentInputColour;
    }

    // Colour.suColour()
    public static String suColour() {
        return currentSuccessColour;
    }

    // Colour.opColour()
    public static String opColour() {
        return currentOptionColour;
    }

    // Colour.erColour()
    public static String erColour() {
        return currentErrorColour;
    }

    // Colour.reColour()
    public static String reColour() {
        return RESET;
    }

    public static void customizeColour(Scanner read) {
        System.out.printf("<CLI Colour Customization>");
        
    }   
}
