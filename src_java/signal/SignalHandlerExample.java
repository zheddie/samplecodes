    import sun.misc.Signal;
    import sun.misc.SignalHandler;
     
    public class SignalHandlerExample implements SignalHandler {
     
        private SignalHandler oldHandler;
 
        static {
                System.load("./libSignalHandlerExample.so");
        }
        public native synchronized String getinfo();
        public native synchronized String signalReg();
 
        public static SignalHandler install(String signalName) {
            Signal diagSignal = new Signal(signalName);
            SignalHandlerExample instance = new SignalHandlerExample();
            instance.oldHandler = Signal.handle(diagSignal, instance);
            return instance;
        }
     
        public void handle(Signal signal) {
            System.out.println("Signal handler called for signal " 
                  + signal);
            try {
     
                signalAction(signal);
     
                // Chain back to previous handler, if one exists
                if (oldHandler != SIG_DFL && oldHandler != SIG_IGN) {
                    System.out.println("Try system handle!");
                    oldHandler.handle(signal);
                }
                System.out.println("This could woulds not be hit!!! ");
     
            } catch (Exception e) {
                System.out.println("handle|Signal handler failed");
            }
        }
     
        public void signalAction(Signal signal) {
            System.out.println("Handling " + signal.getName());
            try {
                if(signal.getName().equals("TERM")){
                  System.out.println("TERM signal processing:call java native interface."); 
                  String info = new SignalHandlerExample().getinfo();
                  System.out.println(info);
                }
                if(signal.getName().equals("INT")){
                  System.out.println("INT signal processing:exit(0)");
                  System.exit(0);
                }
            } catch (Exception e) {
                System.out.println("Interrupted: " 
                  + e.getMessage());
            }
        }
     
        public static void main(String[] args) {
            String info = new SignalHandlerExample().signalReg();
            System.out.println(info);
            SignalHandlerExample.install("TERM");
            SignalHandlerExample.install("INT");
 
            System.out.println("Signal handling example.");
            try {
                Thread.sleep(100000);
            } catch (InterruptedException e) {
                System.out.println("Interrupted: " + e.getMessage());
            }
     
        }
    }
