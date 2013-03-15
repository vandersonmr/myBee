package com.example.teste21;

import java.net.SocketException;
import java.util.LinkedList;

import ufrj.coppe.lcp.repa.PrefixAddress;
import ufrj.coppe.lcp.repa.RepaMessage;
import ufrj.coppe.lcp.repa.RepaSocket;

public class MonitorAbelha {
	public MonitorAbelha(){
		RepaSocket rpa = RepaSocket.getRepaSocket();
		try {
			rpa.repaOpen();
			rpa.registerInterest("cliente");
			//rpa.repaSend(new RepaMessage("app-chat://message","AE FUNFOU".getBytes(),new PrefixAddress()));
			rpa.repaClose();
		} catch (SocketException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	LinkedList<String> listaReq = new LinkedList<String>();
	// Gera dado randomico
	private String gerarDadoTemperatura(){
		return String.valueOf((int)(Math.random()*100))+"º celcius";
	}
	
	
	// Recebe um string e manda pro servidor.
	public void sendMsgToServer(String data){
		
	}
	
	// PARSE da requisição
	private void getData(String data){
		if (data.equals("temperatura")){
			sendMsgToServer(gerarDadoTemperatura());
		}
	}
	
	// Thread que pega fila e executa
	private void trataPedidos(){
		// while LinkedList not null getData
	}
	
	// Thread que ouve servidor e infilera pedido | EU 
	public void listenerMsg(){
		
	}
}
