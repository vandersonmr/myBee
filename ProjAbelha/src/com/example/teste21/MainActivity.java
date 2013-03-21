package com.example.teste21;

import java.net.SocketException;

import ufrj.coppe.lcp.repa.RepaAndroid;
import ufrj.coppe.lcp.repa.RepaSocket;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.app.Activity;
import android.util.Log;
import android.view.Menu;
import android.widget.TextView;

public class MainActivity extends Activity {

	private MonitorAbelha monitor;
	private TextView log;
	
	private void setLog(String s){
			log.setText(s+"\n"+log.getText());
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		try {
			log =new TextView(this);
			log=(TextView)findViewById(R.id.log); 
			log.setFocusable(false);
			log.setFocusableInTouchMode(false);
			
			Handler handler = new Handler(){
	            @Override
	            public void handleMessage(Message msg) {
	                setLog((String)msg.obj);
	            }
	        };

	        monitor = new MonitorAbelha(handler);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	@Override
	protected void onDestroy(){
		super.onDestroy();
		monitor.close();
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	

}
