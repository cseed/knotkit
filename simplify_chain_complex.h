
template<class R> class simplified_complex_generators
{
  unsigned new_n;
  ptr<const module<R> > C;
  basedvector<unsigned, 1> new_C_to_C_generator;
  
public:
  simplified_complex_generators (const simplified_complex_generators &g)
    : new_n(g.new_n),
      C(g.C),
      new_C_to_C_generator(g.new_C_to_C_generator)
  { }
  simplified_complex_generators (unsigned new_n_,
				 ptr<const module<R> > C_,
				 basedvector<unsigned, 1> new_C_to_C_generator_)
    : new_n(new_n_),
      C(C_),
      new_C_to_C_generator(new_C_to_C_generator_)
  { }
  ~simplified_complex_generators () { }
  
  simplified_complex_generators &operator = (const simplified_complex_generators &); // doesn't exist
  
  unsigned dim () const { return new_n; }
  unsigned free_rank () const { return new_n; }
  grading generator_grading (unsigned i) const { return C->generator_grading (new_C_to_C_generator[i]); }
  void show_generator (unsigned i) const { C->show_generator (new_C_to_C_generator[i]); }
  R generator_ann (unsigned i) const { abort (); }
};

template<class R> 
class chain_complex_simplifier
{
 public:
  ptr<const module<R> > C;
  unsigned n; // |C|
  mod_map<R> d;
  
  ptr<const module<R> > new_C;
  mod_map<R> new_d;
  
  // pi : C -> new_C
  mod_map<R> pi;
  
  // iota : new_C -> C
  mod_map<R> iota;
  
 private:
  set<unsigned> canceled;
  
  basedvector<R, 1> cancel_binv;
  basedvector<unsigned, 1> cancel_j;
  basedvector<linear_combination<R>, 1> cancel_di;
  
  basedvector<linear_combination<R>, 1> new_d_columns;
  basedvector<set<unsigned>, 1> preim;
  
  basedvector<linear_combination<R>, 1> iota_columns;
  
  void cancel (unsigned i, R b, unsigned j);
  
 public:
  chain_complex_simplifier (ptr<const module<R> > C_,
			    const mod_map<R> &d_,
			    int dq);
  
};

template<class R> void
chain_complex_simplifier<R>::cancel (unsigned i, R b, unsigned j)
{
  assert (i != j);
  assert (b.is_unit ());
  
  R binv = b.recip ();
  
  canceled.push (i);
  canceled.push (j);
  
  new_d_columns[i].yank (j);
  preim[j].yank (i);
  
  for (linear_combination_const_iter<R> k = new_d_columns[i]; k; k ++)
    preim[k.key ()].yank (i);
  for (set_const_iter<unsigned> k = preim[i]; k; k ++)
    new_d_columns[k.val ()].yank (i);
  for (linear_combination_const_iter<R> k = new_d_columns[j]; k; k ++)
    preim[k.key ()].yank (j);
  
  for (set_const_iter<unsigned> kk = preim[j]; kk; kk ++)
    {
      unsigned k = kk.val ();
      R a = new_d_columns[k](j);
      assert (a != 0);
      
      iota_columns[k].mulsub (a * binv, iota_columns[i]);
    }
  
  iota_columns[i].clear ();
  iota_columns[j].clear ();
  
  for (set_const_iter<unsigned> kk = preim[j]; kk; kk ++)
    {
      unsigned k = kk.val ();
      R a = new_d_columns[k](j);
      assert (a != 0);
      
      R abinv = a * binv;
      
      for (linear_combination_const_iter<R> ll = new_d_columns[i]; ll; ll ++)
	{
	  unsigned ell = ll.key ();
	  R c = ll.val ();
	  
	  assert (! (canceled % k));
	  assert (! (canceled % ell));
	  assert (k != i);
	  assert (k != j);
	  assert (ell != i);
	  assert (ell != j);
	  assert (ell != k);
	  
	  new_d_columns[k].mulsub (abinv * c, ell);
	  if (new_d_columns[k] % ell)
	    preim[ell] += k;
	  else
	    preim[ell] -= k;
	}
    }
  
  for (set_const_iter<unsigned> k = preim[j]; k; k ++)
    new_d_columns[k.val ()].yank (j);
  
  cancel_binv.append (binv);
  cancel_j.append (j);
  cancel_di.append (new_d_columns[i]);
  
  new_d_columns[i] = linear_combination<R> ();
  preim[i].clear ();
  new_d_columns[j].clear ();
  preim[j].clear ();
}

template<class R>
chain_complex_simplifier<R>::chain_complex_simplifier (ptr<const module<R> > C_,
						       const mod_map<R> &d_,
						       int dq)
  : C(C_), n(C_->dim ()), d(d_),
    new_d_columns(n),
    preim(n),
    iota_columns(n)
{
  for (unsigned i = 1; i <= n; i ++)
    {
      new_d_columns[i] = d.column_copy (i);
      
      for (linear_combination_const_iter<R> j = new_d_columns[i]; j; j ++)
	preim[j.key ()].push (i);
      
      linear_combination<R> x (C);
      x.muladd (1, i);
      iota_columns[i] = x;
    }
  
  for (unsigned i = n; i >= 1; i --)
    {
      if (canceled % i)
	continue;
      
      grading igr = C->generator_grading (i);
      for (linear_combination_const_iter<R> j = new_d_columns[i]; j; j ++)
	{
	  grading jgr = C->generator_grading (j.key ());
	  if (j.val ().is_unit ()
	      && jgr.q - igr.q == dq
	      )
	    {
	      cancel (i, j.val (), j.key ());
	      break;
	    }
	}
    }
  
  // ??? might not be completely simplified
  
  unsigned new_n = n - canceled.card ();
  basedvector<unsigned, 1> new_C_to_C_generator (new_n),
    C_to_new_C_generator (n);
  for (unsigned i = 1, j = 1; i <= n; i ++)
    {
      if (canceled % i)
	{
	  C_to_new_C_generator[i] = 0;
	  continue;
	}
      
      C_to_new_C_generator[i] = j;
      new_C_to_C_generator[j] = i;
      j ++;
    }
  
  new_C = (new base_module<R, simplified_complex_generators<R> >
	   (simplified_complex_generators<R> (new_n, C, new_C_to_C_generator)));
  map_builder<R> db (new_C);
  
  for (unsigned i = 1; i <= new_n; i ++)
    {
      unsigned i0 = new_C_to_C_generator[i];
	  
      for (linear_combination_const_iter<R> j0 = new_d_columns[i0]; j0; j0 ++)
	{
	  unsigned j = C_to_new_C_generator[j0.key ()];
	  assert (j != 0);
	  
	  db[i].muladd (j0.val (), j);
	}
    }
  new_d = mod_map<R> (db);
  
  map_builder<R> iotab (new_C, C);
  
  for (unsigned i = 1; i <= new_n; i ++)
    iotab[i] = iota_columns[new_C_to_C_generator[i]];
  iota = mod_map<R> (iotab);

  map_builder<R> pib (C, new_C);
  for (unsigned i = 1; i <= new_n; i ++)
    pib[new_C_to_C_generator[i]].muladd (1, i);
  
  while (cancel_binv.size () > 0)
    {
      R binv = cancel_binv.pop ();
      unsigned j = cancel_j.pop ();
      linear_combination<R> di = cancel_di.pop ();
      
      for (linear_combination_const_iter<R> ll = di; ll; ll ++)
	{
	  R c = ll.val ();
	  pib[j].mulsub (binv * c, pib[ll.key ()]);
	}
    }
  pi = mod_map<R> (pib);
  
  assert (d.compose (iota) == iota.compose (new_d));
  assert (new_d.compose (pi) == pi.compose (d));
  assert (pi.compose (iota) == mod_map<R> (new_C, 1));
}
