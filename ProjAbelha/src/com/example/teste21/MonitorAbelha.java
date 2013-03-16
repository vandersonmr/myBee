package com.example.teste21;

import java.util.LinkedList;

import android.util.Log;

import ufrj.coppe.lcp.repa.PrefixAddress;
import ufrj.coppe.lcp.repa.RepaMessage;
import ufrj.coppe.lcp.repa.RepaSocket;

public class MonitorAbelha {
	private RepaSocket rpa = RepaSocket.getRepaSocket();
	private LinkedList<String> listReq = new LinkedList<String>();
	private Thread execution;
	private Thread listenerMsg;
	public MonitorAbelha() {
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
					trataPedidos();
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

	private String gerarDadoTemperatura() {
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
		}
	}

	private void getData(String data) {
		if (data.equals("temperatura")) {
			sendMsgToServer(gerarDadoTemperatura());
		}
	}

	private void trataPedidos() {
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
					listReq.add(msg.toString());
				}
			}
		} catch (Exception e) {
			// TODO!
			e.printStackTrace();
		}
	}
}
