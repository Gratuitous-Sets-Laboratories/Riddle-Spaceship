void openIris(){

  digitalWrite(iris1Relay,LOW);
  digitalWrite(iris2Relay,HIGH);
  delay(timeToOpen);
}

void closeIris(){

  digitalWrite(iris1Relay, HIGH);
  digitalWrite(iris2Relay, LOW);
  delay(timeToOpen);
}
