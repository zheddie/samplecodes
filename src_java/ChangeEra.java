import java.text.DateFormatSymbols;
import java.text.SimpleDateFormat;
import java.util.Date;

public class ChangeEra {
  public static void main(String s[]) {
    SimpleDateFormat sdf = new SimpleDateFormat();
    DateFormatSymbols dfs = sdf.getDateFormatSymbols();

    String era[] = { "BCE", "CE" };
    dfs.setEras(era);
    sdf.setDateFormatSymbols(dfs);

    sdf.applyPattern("MMMM d yyyy G");
    System.out.println(sdf.format(new Date()));
  }
}
