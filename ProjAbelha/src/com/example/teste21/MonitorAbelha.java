package com.example.teste21;

import java.net.SocketException;
import java.util.LinkedList;

import ufrj.coppe.lcp.repa.PrefixAddress;
import ufrj.coppe.lcp.repa.RepaMessage;
import ufrj.coppe.lcp.repa.RepaSocket;

public class MonitorAbelha {
	RepaSocket rpa = RepaSocket.getRepaSocket();
	LinkedList<String> listaReq = new LinkedList<String>();
	public MonitorAbelha(){
		try {
			listenerMsg();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	// Gera dado randomico
	private String gerarDadoTemperatura(){
		return String.valueOf((int)(Math.random()*100))+"º celcius";
	}
	
	// Recebe um string e manda pro servidor.
	public void sendMsgToServer(String data){
		String interest="servidor";
		int data_length=data.length();
		if (data_length>0){
			try {
				rpa.repaSend(new RepaMessage(interest,data.getBytes(),new PrefixAddress()));
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			System.out.printf("Message sent I: %s | D: %s\n",interest,data);
		}
	}

	// PARSE da requisição
	private void getData(String data){
		if (data.equals("temperatura")){
			sendMsgToServer(gerarDadoTemperatura());
		}
	}
	
	// Thread que pega fila e executa
	private void trataPedidos(){
		while(!listaReq.isEmpty()){
			getData(listaReq.remove());
		}
	}
	
	// Thread que ouve servidor e infilera pedido | EU 
	public void listenerMsg(){
		
	}
}
