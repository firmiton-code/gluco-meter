float A, B;

float uRendah[] = {40, 50, 60};
float uNormal[] = {50, 80, 110};
float uTinggi[] = {100, 150, 200};

float Rendah = 0;
float Normal = 0.5;
float Tinggi = 1;

float minr[4];
float Rule[4];

float fuRendah(int bpm)
{
   if (bpm < uRendah[0])
    {
        return 0;
    }
    else if (bpm >= uRendah[0] && bpm <= uRendah[1])
    {
        return (bpm - uRendah[0]) / (uRendah[1] - uRendah[0]);
    }
    else if (bpm >= uRendah[1] && bpm <= uRendah[2])
    {
        return (uRendah[2] - bpm) / (uRendah[2] - uRendah[1]);
    }
    // else if (bpm > uRendah[2])
    else  {
        return 0;
    }
}

float fuNormal(int bpm)
{
   if (bpm <= uNormal[0])
    {
        return 0;
    }
    else if (bpm >= uNormal[0] && bpm <= uNormal[1])
    {
        return (bpm - uNormal[0]) / (uNormal[1] - uNormal[0]);
    }
    else if (bpm >= uNormal[1] && bpm <= uNormal[2])
    {
        return (uNormal[2] - bpm) / (uNormal[2] - uNormal[1]);
    }
    // else if (bpm > uNormal[2])
    else  {
        return 0;
    }
}

float fuTinggi(int bpm)
{
   if (bpm <= uTinggi[0])
    {
        return 0;
    }
    else if (bpm >= uTinggi[0] && bpm <= uTinggi[1])
    {
        return (bpm - uTinggi[0]) / (uTinggi[1] - uTinggi[0]);
    }
    else if (bpm >= uTinggi[1] && bpm <= uTinggi[2])
    {
        return (uTinggi[2] - bpm) / (uTinggi[2] - uTinggi[1]);
    }
    // else if (bpm >= uTinggi[2])
    else    {
        return 0;
    }
}

void rule2(int bpm)
{
  // if bpm rendah then rendah
  minr[1] = fuRendah(bpm);
  Rule[1] = Rendah;
  // if bpm normal then normal
  minr[2] = fuNormal(bpm);
  Rule[2] = Normal;
  // if bpm tinggi then tinggi
  minr[3] = fuTinggi(bpm);
  Rule[3] = Tinggi;
}

float defuzzyfikasi2(int bpm)
{
  rule2(bpm);
  A = 0;
  B = 0;

  for (int i = 1; i <= 3; i++)
  {
    // printf("Rule ke %d = %f\n", i, Rule[i]);
    // printf("Min ke %d = %f\n", i, minr[i]);
    A += Rule[i] * minr[i];
    B += minr[i];
  }
  // printf("Hasil A : %f\n", A);
  // printf("Hasil B : %f\n", B);
  return A / B;
}

String fuzzy_bpm(int bpm)
{
  float keputusan = defuzzyfikasi2(bpm);
  String msg;

  Serial.print("Hasil Keputusan Defuzzyfikasi : ");
  Serial.println(keputusan);
  if (keputusan >= 0 && keputusan < 0.5) {
    msg = "Rendah";
  } else if (keputusan >= 0.5 && keputusan < 1) {
    msg = "Normal";
  } else if (keputusan >= 1) {
    msg = "Tinggi";
  } else{
    msg = "Error";
  }
  Serial.println("Status_bpm : "+msg);
  return msg;
}
