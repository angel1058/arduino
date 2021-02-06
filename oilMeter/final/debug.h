void log(String msg)
{
  #ifdef _DEBUG_
    Serial.println(msg);
  #endif
}
