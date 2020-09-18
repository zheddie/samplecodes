Java_java_net_PlainDatagramSocketImpl_send(JNIEnv *env, jobject this,
					   jobject packet,          /*ibm@1173*/
					   jbyteArray packetBuffer, /*ibm@1173*/
					   jobject fdObj,           /*ibm@1173*/
					   jobject packetAddress)   /*ibm@1173*/
{                                                                   /*ibm@1173*/
  char *fullPacket = NULL;
  int ret;
  jint packetBufferOffset, packetBufferLen, packetPort;
  /* The packetAddress address, family and port */
  jint address, family, port;
  /* The fdObj'fd */
  jint fd;
  jint trafficClass = (*env)->GetIntField(env, this, pdsi_trafficClassID);
  jboolean connected;
  ssize_t n = -1;
  SOCKADDR rmtaddr, *rmtaddrP=&amp;amp;rmtaddr;
  int len;
  

  fd = (*env)->GetIntField(env, fdObj, IO_fd_fdID);

  connected = (*env)->GetBooleanField(env, this, pdsi_connected);


  packetPort = (*env)->GetIntField(env, packet, dp_portID);
  packetBufferOffset = (*env)->GetIntField(env, packet, dp_offsetID);
  packetBufferLen = (*env)->GetIntField(env, packet, dp_lengthID);

  if (connected)
  {
    /* arg to NET_Sendto () null in this case */
    len = 0;
    rmtaddrP = 0;
  }
  else
  {
    packetPort = (*env)->GetIntField(env, packet, dp_portID);
    if (NET_InetAddressToSockaddr(env, packetAddress, packetPort, (struct sockaddr *)&rmtaddr, &len) != 0)
    {
      return;
    }
  }
  
  fullPacket = (char*)(*env)->GetByteArrayElements(env, packetBuffer, 0);
  /*ibm@793*/
  if (NULL==fullPacket)  /* ibm@72773 */
  {
    
    return;
  }   
  /*
   * Send the datagram.
   *
   * If we are connected it's possible that sendto will return
   * ECONNREFUSED indicating that an ICMP port unreachable has
   * received.
   */
  ret = NET_SendTo(fd, fullPacket+packetBufferOffset, packetBufferLen, 0,
		   /*ibm@793*/
		   (struct sockaddr *)rmtaddrP, len);
  if (ret &amp;lt; 0) {
    
    switch (ret) {
	case JVM_IO_ERR :
	    if (errno == ECONNREFUSED) {
	      JNU_ThrowByName(env, JNU_JAVANETPKG "PortUnreachableException",
	      "ICMP Port Unreachable");
	    } else {
	      NET_ThrowByNameWithLastError(env, "java/io/IOException", "sendto failed");
	    }
	    break;
	case JVM_IO_INTR:
	    JNU_ThrowByName(env, "java/io/InterruptedIOException",
	    "operation interrupted");
	    break;
    }
  }
  (*env)->ReleaseByteArrayElements(env, packetBuffer, (jbyte*)fullPacket, JNI_ABORT);
  /*ibm@61040.1*//*ibm@793*/
  
  return;
}
