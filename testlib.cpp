
#include <knotkit.h>

void
test_vector ()
{
  basedvector<int, 1> v;
  assert (v.size () == 0);
  assert (v.capacity () == 0);
  
  v.append (69);
  assert (v.size () == 1);
  assert (v[1] == 69);
  
  v.append (8);
  v.append (13);
  assert (v.pop () == 13);
  assert (v.pop () == 8);
  v.append (14);
  v.append (17);
  v.append (36);
  v.append (101);
  assert (v.size () == 5);
  assert (v.pop () == 101);
  assert (v.pop () == 36);
  assert (v.pop () == 17);
  assert (v.pop () == 14);
  assert (v.pop () == 69);
  assert (v.size () == 0);
  
  v = basedvector<int, 1> ();
  assert (v.size () == 0);
  assert (v.capacity () == 0);
  
  v.append (69);
  v.append (14);
  v.append (17);
  v.append (36);
  v.append (101);
  assert (v.size () == 5);
  
  v.resize (10);
  assert (v.size () == 10);
  assert (v.capacity () >= v.size ());
  
  assert (v[1] == 69);
  assert (v[2] == 14);
  assert (v[3] == 17);
  assert (v[4] == 36);
  assert (v[5] == 101);
  
  v[1] = 52;
  v[2] = 666;
  v[3] = 55559;
  v[4] = 1721;
  v[5] = -24;
  v[6] = 8893;
  v[7] = 42;
  v[8] = 173;
  v[9] = 115;
  v[10] = 382;
  
  /* shared */
  basedvector<int, 1> v2 (v);
  assert (v2.size () == 10);
  for (int i = 1; i <= 10; i ++)
    assert (v2[i] == v[i]);
  
  assert (v2[8] == 173);
  v[8] = 174;
  assert (v2[8] == 174);
  
  basedvector<int, 1> u (COPY, v);
  assert (u.size () == 10);
  for (int i = 1; i <= 10; i ++)
    assert (u[i] == v[i]);
  
  int x[6] = { 7, 4, 2, 3, 9, 1 };
  basedvector<int, 1> w (4, x);
  assert (w.size () == 4);
  for (int i = 0; i < 4; i ++)
    assert (w[i + 1] == x[i]);
  
  basedvector<int, 1> y (6, v);
  assert (y.size () == 6);
  for (int i = 1; i <= 6; i ++)
    assert (y[i] == v[i]);
  
  y = w;
  assert (y.size () == 4);
  for (int i = 1; i <= 4; i ++)
    assert (y[i] == w[i]);
}

void
test_bitset ()
{
  bitset s;
  assert (s.size () == 0);
  assert (s.card () == 0);
  
  bitset t (100);
  assert (t.size () == 100);
  assert (t.card () == 0);
  t.push (52);
  t.push (1);
  t.push (31);
  t.push (32);
  t.push (100);
  t.push (63);
  t.push (7);
  
  int c = 0;
  for (bitset_const_iter i = t; i; i ++)
    {
      assert (t(i.val ()));
      c ++;
    }
  assert (c == 7);
  
  assert (t.card () == 7);
  
  assert (t(52));
  assert (t(1));
  assert (t(31));
  assert (t(32));
  assert (t(100));
  assert (t(63));
  assert (t(7));
  
  assert (!t(2));
  assert (!t(15));
  assert (!t(47));
  assert (!t(99));
  
  t -= 52;
  t -= 63;
  t -= 7;
  t += 100;
  t += 82;
  t -= 47;
  assert (t.card () == 5);
  assert (!t(52));
  assert (!t(63));
  assert (!t(7));
  assert (!t(47));
  assert (t(100));
  assert (t(82));
  
  bitset u (t);
  assert (u.size () == 100);
  assert (u.card () == 5);
}

template<class unsignedset1> void
test_unsignedset1 ()
{
  unsignedset1 s (94);
  assert (s.card () == 0);
  
  unsignedset1 t (300);
  assert (t.card () == 0);
  t.push (52);
  t.push (1);
  t.push (31);
  t.push (32);
  t.push (33);
  t.push (63);
  t.push (64);
  t.push (65);
  t.push (300);
  t.push (299);
  t.push (243);
  t.push (172);
  t.push (122);
  t.push (70);
  
  int c = 0;
  for (typename unsignedset1::const_iter i = t; i; i ++)
    {
      assert (t(i.val ()));
      c ++;
    }
  assert (c == 14);
  
  assert (t.card () == 14);
  
  assert (t(52));
  assert (t(1));
  assert (t(31));
  assert (t(32));
  assert (t(33));
  assert (t(63));
  assert (t(64));
  assert (t(65));
  assert (t(300));
  assert (t(299));
  assert (t(243));
  assert (t(172));
  assert (t(122));
  assert (t(70));
  
  assert (!t(2));
  assert (!t(15));
  assert (!t(47));
  assert (!t(99));
  assert (!t(127));
  assert (!t(128));
  assert (!t(129));
  
  t -= 52;
  t -= 63;
  t -= 64;
  t -= 65;
  t -= 7;
  t += 100;
  t += 82;
  t -= 47;
  assert (t.card () == 12);
  assert (!t(52));
  assert (!t(63));
  assert (!t(64));
  assert (!t(65));
  assert (!t(7));
  assert (!t(47));
  assert (t(100));
  assert (t(82));
  
  unsignedset1 u (COPY, t);
  assert (u.card () == 12);
}

void
test_map ()
{
  map<int, int> m;
  assert (m.card () == 0);
  m.push (1, 7);
  m.set (2, 9);
  m[3] = 14;
  m[4] = 23;
  assert (m.card () == 4);
  
  assert (m % 1);
  assert (m % 2);
  assert (m % 3);
  assert (m % 4);
  assert (! (m % 5));
  
  assert (m(1) == 7);
  assert (m(2) == 9);
  assert (m(3) == 14);
  assert (m(4) == 23);
  
  m[2] = 17;
  assert (m(2) == 17);

  int c = 0;
  for (map<int, int>::iter i = m; i; i ++)
    {
      int k = i.key ();
      assert (k >= 1);
      assert (k <= 4);
      switch (k)
	{
	case 1: assert (i.val () == 7); break;
	case 2: assert (i.val () == 17); break;
	case 3: assert (i.val () == 14); break;
	case 4: assert (i.val () == 23); break;
	default: abort ();
	}
      c ++;
    }
  assert (c == 4);
  
  map<int, int> n (COPY, m);
  assert (n.card () == 4);
  m[2] = 111;
  assert (n(2) == 17);
}

void
test_polynomial ()
{
  vector<int> coeffs (3);
  coeffs[0] = 7;
  coeffs[1] = 0;
  coeffs[2] = 2;
  polynomial<int> p (coeffs);
  p.show_self ();
  printf ("\n");
  
  polynomial<int> q (coeffs);
  q += p;
  show (q);
  printf ("\n");
  
  q *= p;
  show (q);
  printf ("\n");
}

void
test_laurentpoly ()
{
  vector<int> c (3);
  c[0] = 1;
  c[1] = 0;
  c[2] = 1;
  laurentpoly<int> p (-1, c);
  
  show (p);
  printf ("\n");
  
  p *= p;
  show (p);
  printf ("\n");
}

#if 0
void
test_vs ()
{
  grading gr;
  gr.h = -1;
  gr.q = 1;
  
  basedvector<grading, 1> g5_gr (5);
  g5_gr[1] = g5_gr[2] = g5_gr[3] = g5_gr[4] = g5_gr[5] = gr;
  
  basedvector<ptr<vsgenerator>, 1> gs5;
  gs5.append (new euclideangenerator (1));
  gs5.append (new euclideangenerator (2));
  gs5.append (new euclideangenerator (3));
  gs5.append (new euclideangenerator (4));
  gs5.append (new euclideangenerator (5));
  ptr<const vectorspace<F2> > e5 = new vectorspace<F2> (gs5, g5_gr);
  printf ("e5: ");
  e5->show_self ();
  printf ("\n");
  
  vselt<F2> x1 = e5->singleton (1);
  x1 += e5->singleton (3);

  vselt<F2> x2 = e5->singleton (2);
  x2 += e5->singleton (3);
  x2 += e5->singleton (5);
  
  vselt<F2> x3 = e5->singleton (4);
  x3 += e5->singleton (5);
  
  vselt<F2> x4 = e5->singleton (1);
  x4 += e5->singleton (2);
  x4 += e5->singleton (5);
  
  basedvector<vselt<F2>, 1> xs (4);
  xs[1] = x1;
  xs[2] = x2;
  xs[3] = x3;
  xs[4] = x4;
  
  ptr<const vectorspace<F2> > vs3 = e5->subspace (xs);
  assert (vs3->dim () == 3);
  printf ("vs3: ");
  vs3->show_self ();
  printf ("\n");
  
  basedvector<grading, 1> g2_gr (2);
  g2_gr[1] = g2_gr[2] = gr;
  
  basedvector<ptr<vsgenerator>, 1> gs2;
  gs2.append (new euclideangenerator (1));
  gs2.append (new euclideangenerator (2));
  ptr<const vectorspace<F2> > e2 = new vectorspace<F2> (gs2, g2_gr);

  basedvector<grading, 1> g3_gr (3);
  g3_gr[1] = g3_gr[2] = g3_gr[3] = gr;
  
  basedvector<ptr<vsgenerator>, 1> gs3;
  gs3.append (new euclideangenerator (1));
  gs3.append (new euclideangenerator (2));
  gs3.append (new euclideangenerator (3));
  ptr<const vectorspace<F2> > e3 = new vectorspace<F2> (gs3, g3_gr);
  
  basedvector<vselt<F2>, 1> cols (3);
  cols[1] = e2->singleton (1);
  cols[2] = e2->singleton (2);
  vselt<F2> x5 = e2->singleton (1);
  x5 += e2->singleton (2);
  cols[3] = x5;
  vsmap<F2> m (e3, e2, cols);
  
  ptr<const vectorspace<F2> > mker = m.kernel ();
  printf ("mker: ");
  mker->show_self ();
  printf ("\n");
}
#endif

void
test_fields ()
{
  Q a (2, 1);
  Q b (-2, 1);
  a /= b;
  assert (a == -1);
}

int
main ()
{
  test_vector ();
  test_bitset ();
  test_unsignedset1<unsignedset> ();
  test_unsignedset1<bitset> ();
  test_unsignedset1<ullmanset<1> > ();
  test_map ();
  test_polynomial ();
  test_laurentpoly ();
  // test_vs ();
  test_fields ();
}
