import java.rmi.*;

import java.rmi.server.*;
import java.rmi.registry.*;


public class Hello extends UnicastRemoteObject implements HelloInterface {

	private String message;


	public Hello (String msg) throws RemoteException {

		message = msg;

	}


	public String say() throws RemoteException {

		return message;

	}

	public static void main (String[] argv) {

		try {
			Registry rgsty = LocateRegistry.createRegistry(8808);
			rgsty.rebind("Hello",new Hello ("Hello, world!"));
			//Naming.rebind ("//localhost:8808/Hello", new Hello ("Hello, world!"));

				System.out.println ("Hello Server is ready.");

		} catch (Exception e) {

			System.out.println ("Hello Server failed: " + e);

		}

	}
}
