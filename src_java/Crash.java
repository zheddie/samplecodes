public class Crash {
    public static void main(String[] args) {
        Object[] o = null;

        while (true) {
            o = new Object[] {o};
        }
    }
}
