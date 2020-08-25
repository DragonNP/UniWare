void logFailedOpen(String function_name, String file_name) {
  Serial.print("(");
  Serial.print(function_name);
  Serial.print(") ");
  Serial.print("FAIL: Failed to open ");
  Serial.print(file_name);
  Serial.print(" file");
}

void logFileLarge(String function_name, String file_name) {
  Serial.print("(");
  Serial.print(function_name);
  Serial.print(") ");
  Serial.print("FAIL: ");
  Serial.print(file_name);
  Serial.println(" file size is too large");
}

void logFailedParse(String function_name, String var, String name_var, String error, size_t capacity) {
  Serial.print("(");
  Serial.print(function_name);
  Serial.print(") ");
  Serial.print("FAIL: Failed to parse ");
  Serial.print(name_var);
  Serial.print(": ");
  Serial.print("error: ");
  Serial.print(error);
  Serial.print(", capacity: ");
  Serial.print(capacity);
  Serial.print(", var: ");
  Serial.println(var);
}
