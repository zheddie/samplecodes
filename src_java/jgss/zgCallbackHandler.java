import java.io.PrintWriter;
import java.io.IOException;
import javax.security.auth.callback.Callback;
import javax.security.auth.callback.CallbackHandler;
import javax.security.auth.callback.TextOutputCallback;
import javax.security.auth.callback.NameCallback;
import javax.security.auth.callback.PasswordCallback;
import javax.security.auth.callback.UnsupportedCallbackException;
// allow the user to programmatically entered the userName and Password
// through another routine call instead of through the command line.

public class zgCallbackHandler implements CallbackHandler {

    String userName = null;
    String passWord = null;
    char[] pw = null;

    //
    // caller has to provide the printout mechanism, the user name
    // and the password.
    //
    public zgCallbackHandler(String userName, String passWord) {
        this.userName = userName;
        this.passWord = passWord;
	// convert the password string into byte array
        pw = new char[passWord.length()];
        passWord.getChars(0, passWord.length(), pw, 0);
    }


    public void handle(Callback[] callbacks)
        throws IOException, UnsupportedCallbackException {
        for (int i = 0; i < callbacks.length; i++) {
            if (callbacks[i] instanceof TextOutputCallback) {
                // display the message according to the specified type
                TextOutputCallback toc = (TextOutputCallback)callbacks[i];
                switch (toc.getMessageType()) {
                case TextOutputCallback.INFORMATION:
                    System.out.println(toc.getMessage());
                    break;
                case TextOutputCallback.ERROR:
                    System.out.println("ERROR: " + toc.getMessage());
                    break;
                case TextOutputCallback.WARNING:
                    System.out.println("WARNING: " + toc.getMessage());
                    break;
                default:
                    throw new IOException("Unsupported message type: " +
                                          toc.getMessageType());
                }
		
            } else if (callbacks[i] instanceof NameCallback) {
                System.out.println("zgCallbackHandler, callbacks is NameCallback.");
                // prompt the user for a username
                NameCallback nc = (NameCallback)callbacks[i];
                // ignore the provided defaultName
                System.out.println(nc.getPrompt() + " " + userName);
                nc.setName(userName);
		
            } else if (callbacks[i] instanceof PasswordCallback) {
		        System.out.println("zgCallbackHandler, callbacks is PasswordCallback.");
                // set the password input field with the known password
                PasswordCallback pc = (PasswordCallback)callbacks[i];
                System.out.println(pc.getPrompt() + " " + passWord);
                pc.setPassword(pw);
		
            } else {
                throw new UnsupportedCallbackException
		    (callbacks[i], "Unrecognized Callback");
            }
        }
    }
}