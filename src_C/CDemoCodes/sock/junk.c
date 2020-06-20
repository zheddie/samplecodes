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
  Tr_PlainDatagramSocketImpl_send_Entry(env);
  if (IS_NULL(fdObj)) {
    JNU_ThrowByName(env, JNU_JAVANETPKG "SocketException","Socket closed");
    Tr_PlainDatagramSocketImpl_send_Exit1(env);
    return;
  }
  fd = (*env)->GetIntField(env, fdObj, IO_fd_fdID);
  if (IS_NULL(packet)) {
    JNU_ThrowNullPointerException(env, "packet");
    Tr_PlainDatagramSocketImpl_send_Exit2(env, fd);
    return;
  }
  connected = (*env)->GetBooleanField(env, this, pdsi_connected);
  if (IS_NULL(packetBuffer) || IS_NULL(packetAddress)) {
    JNU_ThrowNullPointerException(env, "null buffer || null address");
    return;
  }
  packetPort = (*env)->GetIntField(env, packet, dp_portID);
  packetBufferOffset = (*env)->GetIntField(env, packet, dp_offsetID);
  packetBufferLen = (*env)->GetIntField(env, packet, dp_lengthID);
  if (connected) {
    /* arg to NET_Sendto () null in this case */
    len = 0;
    rmtaddrP = 0;
  } else {
    packetPort = (*env)->GetIntField(env, packet, dp_portID);
    if (NET_InetAddressToSockaddr(env, packetAddress, packetPort, (struct sockaddr *)&amp;amp;rmtaddr, &amp;amp;len) != 0) {
      return;
    }
  }
  fullPacket = (char*)(*env)->GetByteArrayElements(env, packetBuffer, 0);
  /*ibm@793*/
  if (NULL==fullPacket)  /* ibm@72773 */
  {
    Tr_PlainDatagramSocketImpl_send_Exit9(env);
    return;
  }   
#ifdef AF_INET6
  if (trafficClass != 0 &amp;amp;&amp;amp; ipv6_available()) {
    NET_SetTrafficClass((struct sockaddr *)&amp;amp;rmtaddr, trafficClass);
  }
#endif /* AF_INET6 */
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
    Tr_PlainDatagramSocketImpl_send_Event(env, fd, ret, errno, strerror(errno));
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
  Tr_PlainDatagramSocketImpl_send_Exit4(env, fd);
  return;
}
