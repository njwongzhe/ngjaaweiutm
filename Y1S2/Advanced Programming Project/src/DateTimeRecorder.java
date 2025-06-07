package src;
import java.time.LocalDateTime; // Import the LocalDateTime class
import java.time.format.DateTimeFormatter; // Import the DateTimeFormatter class

public class DateTimeRecorder {
    private static final DateTimeFormatter formatter = 
        DateTimeFormatter.ofPattern("dd-MM-yyyy HH:mm:ss");

    public static String getDateTime() {
        return LocalDateTime.now().format(formatter);
    }

    public static int compareDateTime(String dateTime1, String dateTime2) {
        LocalDateTime time1 = LocalDateTime.parse(dateTime1, formatter);
        LocalDateTime time2 = LocalDateTime.parse(dateTime2, formatter);
        return time1.compareTo(time2);
    }
}