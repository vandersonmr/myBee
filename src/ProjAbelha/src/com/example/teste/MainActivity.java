package com.example.teste;

import java.net.SocketException;

import ufrj.coppe.lcp.repa.RepaAndroid;
import ufrj.coppe.lcp.repa.RepaSocket;
import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		RepaSocket rpa = RepaSocket.getRepaSocket();
		try {
			rpa.repaOpen();
			rpa.registerInterest("apt-chat://message");
			rpa.repaClose();
		} catch (SocketException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}
