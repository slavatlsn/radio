class Sender
{
  public:
    Sender(int period);
    void attach(int pin);
    void send(byte b);
  private:
    int T, att;
};

Sender::Sender(int period)
{
  T = period;
}

void Sender::attach(int pin)
{
  att = pin;
  pinMode(att, OUTPUT);
}

void Sender::send(byte b)
{
  digitalWrite(att, HIGH);
  delay(2*T);
  digitalWrite(att, LOW);
  delay(2*T);
  for(int i = 0; i < 8; i++)
  {
    int val = bitRead(b, i);
    digitalWrite(att, val);
    delay(T);
  }
  digitalWrite(att, HIGH);
  delay(2*T);
  digitalWrite(att, LOW);
  delay(2*T+100);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

String rep(String v, int t)
{
  String r = "";
  for(int i = 0; i < t; i++) r += v;
  return r;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

class Receiver
{
  public:
    Receiver(int period);
    void attach(int pin);
    bool has_data();
    String read();
  private:
    int att, T, prev, c, s;
    bool co, dat;
    long int _dat[20];
    long int t0, _t;
    String _byte;
};

Receiver::Receiver(int period)
{
  T = period;
  prev = 0;
  s = 0;
  _t = 0;
  t0 = micros();
  co = false;
  dat = false;
  _byte = "null";
  for(int i = 0; i < 20; i++) _dat[i] = 0;
}

void Receiver::attach(int pin)
{
  att = pin;
  pinMode(att, INPUT);
}

bool Receiver::has_data()
{
  bool has_data = false;
  String res = "";
  int val = digitalRead(att);
  if(val == 1 && prev == 0)
  {
    c = 0;
    co = true;
    _t = micros();
  }
  if(val == 1 && co) c++;
  if(val == 0 && prev == 1 && co)
  {
    if(c > 100) 
    {
      long int d = micros() - t0;
      t0 = micros();
      if(abs(d - 100000 - 4000*T) < 1000) 
      {
        dat = true;
        int _val = 0;
        for(int i = 2; i < 20; i++) 
        {
          if(_dat[i] != 0) res = rep(String(_val), round((float)(_dat[i] - _dat[i-1])/(1000*T))) + res;
          _val = 1 - _val;
        }
        res = res.substring(2, res.length() - 2);
        has_data = res.length() == 8;
        for(int i = 0; i < 20; i++) _dat[i] = 0;
        s = 0;
      }
      _dat[s] = _t;
      s++;
      _dat[s] = micros();
      s++;
    }
    co = false;
  }
  prev = val;
  if(has_data) _byte = res; else _byte = "null";
  return has_data;
}

String Receiver::read()
{
  return _byte;
}
