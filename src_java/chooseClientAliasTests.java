/*
 * Copyright (c) 2004, 2008, Oracle and/or its affiliates. All rights reserved.
 */

/*
 *   @(#)chooseClientAlias.test.xml 08/03/26 Alexey Ivashchenko, Maxim V. Sokolnikov>
 * Generated from : api/javax_net/ssl/X509KeyManager/chooseClientAlias.test.xml
 *
 * X509KeyManager tests
 */

package javasoft.sqe.tests.api.javax.net.ssl.X509KeyManager;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.MalformedURLException;
import java.net.Socket;
import java.security.KeyStore;
import java.security.Principal;
import javasoft.sqe.javatest.Status;
import javasoft.sqe.jck.lib.javax.net.ssl.Closable;
import javasoft.sqe.jck.lib.javax.net.ssl.Closables;
import javax.net.ssl.X509KeyManager;
import javax.security.auth.x500.X500Principal;


public class chooseClientAliasTests extends KeyManagerTest {

    public Status checkNullClientAlias(final String[] keyType,
                                       final Principal[] issuers,
                                       final Socket socket, KeyStore keystore) {
        return iterateX509KeyManagers(
            new Command() {
                public Status execute(X509KeyManager manager) {
                    String alias = manager.chooseClientAlias(keyType, issuers, socket);
                    return (alias == null) ? PASSED
                           : Status.failed("Returned value is not null");
            }
        }, keystore);
    }

    public Status checkNullClientAlias(final String[] keyType,
                                       final Principal[] issuers,
                                       final Socket socket) {
        return checkNullClientAlias(keyType, issuers, socket, keystore.keystore);
    }

    public Status checkNonNullClientAlias(final String[] keyType,
                                          final Principal[] issuers,
                                          final Socket socket,
                                          final boolean canBeNull) {
        return iterateX509KeyManagers(
            new Command() {
                public Status execute(X509KeyManager manager) {
                    String alias = manager.chooseClientAlias(keyType, issuers, socket);
                    if (alias == null) {
                        return canBeNull ? PASSED : Status.failed("Returned value is null");
                    }
                    String[] aliases = manager.getClientAliases(keyType[0], (Principal[])null);
                    for (int i = 0; i < aliases.length; i++) {
                        if (KeyManagerTest.equals(manager, aliases[i], alias)) {
                            return PASSED;
                        }
                    }
                    return Status.failed("Returned alias name is not present in the list" +
                                         " of all client alias names.");
                }
            }, keystore.keystore);
    }

    /* standalone interface */
    public static void main(String argv[]) {
        chooseClientAliasTests test = new chooseClientAliasTests();
        test.run(argv, System.err, System.out).exit();
    }

    /**
     * Equivalence class partitioning
     * with input and output values orientation
     * for public String chooseClientAlias(String[] keyType, Principal[] issuers, Socket socket),
     * <br><b>keyType</b>: array of size 0
     * <br><b>issuers</b>: null
     * <br><b>socket</b>: null
     * <br><b>output</b>: null.
     */
    public Status chooseClientAlias001() {
        return checkNullClientAlias(new String[0], null, null);
    }

    /**
     * Equivalence class partitioning
     * with input and output values orientation
     * for public String chooseClientAlias(String[] keyType, Principal[] issuers, Socket socket),
     * <br><b>keyType</b>: null
     * <br><b>issuers</b>: null
     * <br><b>socket</b>: null
     * <br><b>output</b>: null.
     */
    public Status chooseClientAlias002() {
        return checkNullClientAlias(null, null, null);
    }

    /**
     * Equivalence class partitioning
     * with input and output values orientation
     * for public String chooseClientAlias(String[] keyType, Principal[] issuers, Socket socket),
     * <br><b>keyType</b>: bad names
     * <br><b>issuers</b>: null
     * <br><b>socket</b>: null
     * <br><b>output</b>: null.
     */
    public Status chooseClientAlias003() {
        final String[] keyType = {
            "a",
            "abcdefg",
            "name with spaces",
            "very long name of key type that actually does not exist"};

        return checkNullClientAlias(keyType, null, null);
    }

    /**
     * Equivalence class partitioning
     * with input and output values orientation
     * for public String chooseClientAlias(String[] keyType, Principal[] issuers, Socket socket),
     * <br><b>keyType</b>: valid name
     * <br><b>issuers</b>: null
     * <br><b>socket</b>: null
     * <br><b>output</b>: alias name.
     */
    public Status chooseClientAlias004() {
        String[] keyType = {getFirstKeyAlgorithm(), getSecondKeyAlgorithm(), "DH_RSA", "DH_DSA"};

        return checkNonNullClientAlias(keyType, null, null, false);
    }

    /**
     * Equivalence class partitioning
     * with input and output values orientation
     * for public String chooseClientAlias(String[] keyType, Principal[] issuers, Socket socket),
     * <br><b>keyType</b>: valid name
     * <br><b>issuers</b>: bad name
     * <br><b>socket</b>: null
     * <br><b>output</b>: null.
     */
    public Status chooseClientAlias005() {
        final String[] keyType = {getFirstKeyAlgorithm(), getSecondKeyAlgorithm(), "DH_RSA", "DH_DSA"};
        final Principal[] issuers = {
            new X500Principal("CN=a"),
            new X500Principal("CN=abcdefg"),
            new X500Principal("CN=name with spaces"),
            new X500Principal("CN=a very long name which actually does not exist")
        };

        return checkNullClientAlias(keyType, issuers, null);
    }

    /**
     * Equivalence class partitioning
     * with input and output values orientation
     * for public String chooseClientAlias(String[] keyType, Principal[] issuers, Socket socket),
     * <br><b>keyType</b>: valid name
     * <br><b>issuers</b>: valid name
     * <br><b>socket</b>: null
     * <br><b>output</b>: alias name.
     */
    public Status chooseClientAlias006() {
        String[] keyType = {
                getFirstKeyAlgorithm(), getSecondKeyAlgorithm(), "DH_RSA", "DH_DSA"
        };
        String issuerName = getIssuerName();
        Principal[] issuers = { new X500Principal(issuerName) };

        return checkNonNullClientAlias(keyType, issuers, null, false);
    }

    /**
     * Equivalence class partitioning
     * with input and output values orientation
     * for public String chooseClientAlias(String[] keyType, Principal[] issuers, Socket socket),
     * <br><b>keyType</b>: valid name
     * <br><b>issuers</b>: null
     * <br><b>socket</b>: unconnected socket
     * <br><b>output</b>: null or alias name.
     */
    public Status chooseClientAlias007() {
        String[] keyType = {getFirstKeyAlgorithm(), getSecondKeyAlgorithm(), "DH_RSA", "DH_DSA"};
        Closables bundle = new Closables();
        try {
            Socket socket = bundle.add(new Socket());
            return checkNonNullClientAlias(keyType, null, socket, true);
        } finally {
            bundle.close();
        }
    }

    /**
     * Equivalence class partitioning
     * with input and output values orientation
     * for public String chooseClientAlias(String[] keyType, Principal[] issuers, Socket socket),
     * <br><b>keyType</b>: valid name
     * <br><b>issuers</b>: null
     * <br><b>socket</b>: connected socket
     * <br><b>output</b>: null or alias name.
     */
    public Status chooseClientAlias008() {
        String[] keyType =
                { getFirstKeyAlgorithm(), getSecondKeyAlgorithm(), "DH_RSA",
                    "DH_DSA" };
        Socket socket = null;
        SocketThread thread = new SocketThread(log);
        Closables bundle = new Closables();

        try {
            thread.start();
            thread.flag.acquire();
            socket = bundle.add(new Socket("localhost", port));
            return checkNonNullClientAlias(keyType, null, socket, true);
        } catch (InterruptedException ie) {
            ie.printStackTrace(log);
            return Status.failed("Unexpected exception: " + ie);
        } catch (IOException ioe) {
            ioe.printStackTrace(log);
            return Status.failed("Unexpected exception: " + ioe);
        } finally {
            thread.flag.release();
            bundle.close();

            try {
                thread.join();
            } catch (InterruptedException ex) {
                ex.printStackTrace(log);
                thread.interrupt();
            }
        }
    }

    /**
     * Equivalence class partitioning
     * with input and output values orientation
     * for public String chooseClientAlias(String[] keyType, Principal[] issuers, Socket socket),
     * <br><b>keyType</b>: array of size 0
     * <br><b>issuers</b>: null
     * <br><b>socket</b>: null
     * <br><b>output</b>: null.
     */
    public Status chooseClientAlias009() {
        return checkNullClientAlias(new String[0], null, null, null);
    }

    /**
     * Equivalence class partitioning
     * with input and output values orientation
     * for public String chooseClientAlias(String[] keyType, Principal[] issuers, Socket socket),
     * <br><b>keyType</b>: null
     * <br><b>issuers</b>: null
     * <br><b>socket</b>: null
     * <br><b>output</b>: null.
     */
    public Status chooseClientAlias010() {
        return checkNullClientAlias(null, null, null, null);
    }

    /**
     * Equivalence class partitioning
     * with input and output values orientation
     * for public String chooseClientAlias(String[] keyType, Principal[] issuers, Socket socket),
     * <br><b>keyType</b>: bad names
     * <br><b>issuers</b>: null
     * <br><b>socket</b>: null
     * <br><b>output</b>: null.
     */
    public Status chooseClientAlias011() {
        String[] keyType = {
            "a",
            "abcdefg",
            "name with spaces",
            "very long name of key type that actually does not exist"};

        return checkNullClientAlias(keyType, null, null, null);
    }

    /**
     * Equivalence class partitioning
     * with input and output values orientation
     * for public String chooseClientAlias(String[] keyType, Principal[] issuers, Socket socket),
     * <br><b>keyType</b>: valid name
     * <br><b>issuers</b>: bad name
     * <br><b>socket</b>: null
     * <br><b>output</b>: null.
     */
    public Status chooseClientAlias012() {
        String[] keyType = {getFirstKeyAlgorithm(), getSecondKeyAlgorithm(), "DH_RSA", "DH_DSA"};
        Principal[] issuers = {
            new X500Principal("CN=a"),
            new X500Principal("CN=abcdefg"),
            new X500Principal("CN=name with spaces"),
            new X500Principal("CN=a very long name which actually does not exist")
        };

        return checkNullClientAlias(keyType, null, null, null);
    }
}
