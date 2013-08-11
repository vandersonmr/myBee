/*package com.example.abelhas;

import java.util.LinkedList;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

import ufrj.coppe.lcp.repa.PrefixAddress;
import ufrj.coppe.lcp.repa.RepaMessage;
import ufrj.coppe.lcp.repa.RepaSocket;

public class MonitorAbelha {
	private RepaSocket rpa = RepaSocket.getRepaSocket();
	private LinkedList<String> listReq = new LinkedList<String>();
	private Thread execution;
	private Thread listenerMsg;
	private Handler handler;
	private Message message = new Message();
	
	private void log(String s){
		message = new Message();
		message.obj = s;
		handler.sendMessage(message);
	}
	
	public MonitorAbelha(Handler handler) {
		this.handler = handler;
		try {
			initRepa();
			
			listenerMsg = new Thread(new Runnable() {
				
				@Override
				public void run() {
					// TODO Auto-generated method stub
					listenerMsg();
				}
			});
			listenerMsg.setPriority(Thread.MAX_PRIORITY);
			listenerMsg.start();
			
			execution = new Thread(new Runnable() {
				
				@Override
				public void run() {
					processRequest();
				}
			});
			execution.setPriority(Thread.MIN_PRIORITY);
			execution.start();
			
			
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	private void initRepa() {
		try {
			rpa.repaOpen();
			rpa.registerInterest("client");
			log("Setado interesse em client");
		} catch (Exception e) {
			// TODO!
			e.printStackTrace();
		}
	}

	private void closeRepa() {
		rpa.repaClose();
	}
	
	public void close(){
		listenerMsg.stop();
		execution.stop();
		closeRepa();
	}

	private String getTemperature() {
		return String.valueOf((int) (Math.random() * 100)) + "º celcius";
	}

	private void sendMsgToServer(String data) {
		String interest = "server";
		int data_length = data.length();
		if (data_length > 0) {
			try {
				rpa.repaSend(new RepaMessage(interest, data.getBytes(),
						new PrefixAddress()));
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			Log.i("Client","Message sent I: "+interest+" | D: "+data+"\n");
			log("Mensagem enviada I: "+interest+" | D: "+data+"\n");
		}
	}

	private void getData(String data) {
		if (data.equals("temperatura")) {
			sendMsgToServer(getTemperature());
		}
	}

	private void processRequest() {
		while (true) {
			if(!listReq.isEmpty()){
				getData(listReq.remove());
			}
		}
	}

	private void listenerMsg() {
		try {
			while (true) {
				RepaMessage msg = rpa.repaRecv();
				if (msg != null) {
					listReq.add(new String(msg.getData()));
				}
			}
		} catch (Exception e) {
			// TODO!
			e.printStackTrace();
		}
	}
}*/
