
#include <knotkit.h>

knot_diagram::knot_diagram (const planar_diagram &pd)
  : name(pd.name),
    n_crossings(pd.crossings.size ()),
    marked_edge(0),
    crossings(n_crossings),
    ept_crossing(num_epts ()),
    ept_index(num_epts ()),
    nminus(0), nplus(0)
{
  basedvector<basedvector<pair<unsigned, unsigned>, 1>, 1> edge_ci (num_edges ());
  for (unsigned c = 1; c <= n_crossings; c ++)
    {
      for (unsigned i = 1; i <= 4; i ++)
	{
	  unsigned e = pd.crossings[c][i];
	  edge_ci[e].append (pair<unsigned, unsigned> (c, i));
	}
      
      crossings[c] = basedvector<unsigned, 1> (4);
    }

  set<unsigned> done;
  for (unsigned c0 = 1; c0 <= n_crossings; c0 ++)
    {
      for (unsigned i0 = 1; i0 <= 2; i0 ++)
	{
	  unsigned e = pd.crossings[c0][i0];
	  if (done % e)
	    continue;
	  
	  for (unsigned c = c0, i = i0;;)
	    {
	      crossings[c][i] = edge_to_ept (e);
	      done.push (e);
	      ept_crossing[edge_to_ept (e)] = c;
	      ept_index[edge_to_ept (e)] = i;
	      
	      i = add_base1_mod4 (i, 2);
	      e = pd.crossings[c][i];
	      crossings[c][i] = edge_from_ept (e);
	      ept_crossing[edge_from_ept (e)] = c;
	      ept_index[edge_from_ept (e)] = i;
	      
	      if (edge_ci[e][1].first == c
		  && edge_ci[e][1].second == i)
		{
		  c = edge_ci[e][2].first;
		  i = edge_ci[e][2].second;
		}
	      else
		{
		  assert (edge_ci[e][2].first == c && edge_ci[e][2].second == i);
		  c = edge_ci[e][1].first;
		  i = edge_ci[e][1].second;
		}
	      
	      if (c == c0 && i == i0)
		break;
	    }
	}
    }
  assert (done.card () == num_edges ());
  
  calculate_smoothing_orientation ();
  calculate_nminus_nplus ();
}

class dt_layout
{
public:
  knot_diagram &kd;

  unsigned n_components;
  basedvector<unsigned, 1> comp_first_edge,
    comp_last_edge;
  basedvector<unsigned, 1> edge_to_crossing;
  smallbitset edge_to_under;
  smallbitset placed;
  
public:
  dt_layout (const dt_code &dt, knot_diagram &kd_);
  dt_layout (const dt_layout &) = delete;
  ~dt_layout () { }

  dt_layout &operator = (const dt_layout &) = delete;
  
  unsigned find_crossing (unsigned prevc, unsigned previ, unsigned target,
			  bool under, unsigned dir);
  
  bool layout_2 (unsigned e,
		 unsigned prevc, unsigned previ,
		 unsigned c, unsigned i);
  bool layout_1 (unsigned e,
		 unsigned prevc, unsigned previ);
  unsigned comp_next_edge (unsigned e) const;
  
  void layout ();
};

dt_layout::dt_layout (const dt_code &dt, knot_diagram &kd_)
  : kd(kd_),
    n_components(dt.num_components ()),
    comp_first_edge(n_components),
    comp_last_edge(n_components),
    edge_to_crossing(kd.num_edges ()),
    edge_to_under(kd.num_edges ()),
    placed(kd.n_crossings)
{
  unsigned k = 1;
  for (unsigned i = 1; i <= dt.even_labels.size (); i ++)
    {
      comp_first_edge[i] = 2 * k - 1;
      comp_last_edge[i] = 2 * (k + dt.even_labels[i].size () - 1);
      for (unsigned j = 1; j <= dt.even_labels[i].size (); j ++, k ++)
	{
	  int as = dt.even_labels[i][j];
	  unsigned a = abs (as),
	    b = 2 * k - 1;
	  if (as > 0)
	    edge_to_under.push (a);
	  else
	    edge_to_under.push (b);
	  
	  edge_to_crossing[a] = edge_to_crossing[b] = k;
	}
    }
}

unsigned
dt_layout::find_crossing (unsigned prevc, unsigned previ, unsigned target,
			  bool under, unsigned dir)
{
  assert (!kd.crossings[prevc][previ]);
  assert (placed % prevc);
  
  unsigned c = prevc,
    i = add_base1_mod4 (previ, dir);
  for (;;)
    {
      if (c == prevc && i == previ)
	return 0;
      
      if (c == target && under == (i == 1 || i == 3) && kd.crossings[c][i] == 0)
	return i;
      unsigned p = kd.crossings[c][i];
      if (p)
	{
	  unsigned p2 = kd.edge_other_ept (p);
	  c = kd.ept_crossing[p2];
	  i = kd.ept_index[p2];
	}
      i = add_base1_mod4 (i, dir);
    }
}

unsigned
dt_layout::comp_next_edge (unsigned e) const
{
  for (unsigned comp = 1; comp <= n_components; comp ++)
    {
      if (comp_first_edge[comp] <= e
	  && e <= comp_last_edge[comp])
	{
	  if (e == comp_last_edge[comp])
	    return comp_first_edge[comp];
	  else
	    return e + 1;
	}
    }
  abort ();
}

bool
dt_layout::layout_2 (unsigned e,
		     unsigned prevc, unsigned previ,
		     unsigned c, unsigned i)
{
  assert (placed % prevc);
  assert (placed % c);
  
  unsigned from = kd.edge_from_ept (e),
    to = kd.edge_to_ept (e);
  
  assert (kd.crossings[prevc][previ] == 0);
  kd.crossings[prevc][previ] = from;
  kd.ept_crossing[from] = prevc;
  kd.ept_index[from] = previ;
  
  assert (kd.crossings[c][i] == 0);
  kd.crossings[c][i] = to;
  kd.ept_crossing[to] = c;
  kd.ept_index[to] = i;
  
  if (layout_1 (comp_next_edge (e), c, add_base1_mod4 (i, 2)))
    return 1;
  
  kd.crossings[prevc][previ] = 0;
  kd.crossings[c][i] = 0;
  kd.ept_crossing[from] = kd.ept_crossing[to] = 0;
  kd.ept_index[from] = kd.ept_index[to] = 0;
  return 0;
}

bool
dt_layout::layout_1 (unsigned e, unsigned prevc, unsigned previ)
{
  if (kd.crossings[prevc][previ])
    {
      unsigned from = kd.edge_from_ept (e);
      
      assert (kd.crossings[prevc][previ] == from);
      assert (kd.ept_crossing[from] == prevc);
      assert (kd.ept_index[from] == previ);
      
      for (unsigned e = 1; e <= kd.num_edges (); e ++)
	{
	  unsigned to = kd.edge_to_ept (e);
	  unsigned prevc = edge_to_crossing[e];
	  if (placed % prevc
	      && !kd.ept_crossing[to])
	    {
	      unsigned previ1 = edge_to_under % e ? 3 : 4;
	      if (layout_1 (comp_next_edge (e), prevc, previ1))
		return 1;
	      
	      unsigned previ2 = edge_to_under % e ? 1 : 2;
	      return layout_1 (comp_next_edge (e), prevc, previ2);
	    }
	}

      /* done! */
      return 1;
    }
  
  unsigned c = edge_to_crossing[e];
  if (placed % c)
    {
      unsigned i = find_crossing (prevc, previ, c, edge_to_under % e, 1);
      if (i && layout_2 (e, prevc, previ, c, i))
	return 1;
	  
      unsigned i2 = find_crossing (prevc, previ, c, edge_to_under % e, 3);
      return i2 && layout_2 (e, prevc, previ, c, i2); // return i && ... ??
    }
  else
    {
      unsigned i = (edge_to_under % e) ? 1 : 2;
      placed.push (c);
      if (!layout_2 (e, prevc, previ, c, i))
	{
	  placed -= c;
	  return 0;
	}
      else
	return 1;
    }
}

void
dt_layout::layout ()
{
  unsigned last = comp_last_edge[1];
  unsigned prevc = edge_to_crossing[last];
  unsigned previ = (edge_to_under % last) ? 3 : 4;
  
  placed.push (prevc);
  bool b = layout_1 (comp_first_edge[1], prevc, previ);
  assert (b);
  assert (placed.card () == kd.n_crossings);
}

knot_diagram::knot_diagram (const dt_code &dt)
  : name(dt.name),
    n_crossings(dt.num_crossings ()),
    marked_edge(0),
    crossings(n_crossings),
    ept_crossing(num_epts ()),
    ept_index(num_epts ()),
    nminus(0), nplus(0)
{
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      basedvector<unsigned, 1> v (4);
      v[1] = v[2] = v[3] = v[4] = 0;
      crossings[i] = v;
    }
  
  dt_layout layout (dt, *this);
  layout.layout ();
  
  calculate_nminus_nplus ();
  calculate_smoothing_orientation ();
}

knot_diagram::knot_diagram (sublink,
			    smallbitset c,
			    const knot_diagram &kd)
  : name(kd.name),
    n_crossings(0),
    marked_edge(0),
    nminus(0),
    nplus(0)
{
  // ??? assert (!kd.marked_edge);
  assert (c.card () > 0);  // no empty diagrams
  
  // edge x component
  unionfind<1> u (kd.num_edges ());
  
  for (unsigned i = 1; i <= kd.n_crossings; i ++)
    {
      u.join (kd.ept_edge (kd.crossings[i][1]),
	      kd.ept_edge (kd.crossings[i][3]));
      u.join (kd.ept_edge (kd.crossings[i][2]),
	      kd.ept_edge (kd.crossings[i][4]));
    }
  
  assert (u.num_sets () == c.size ());
  
  ullmanset<1> u_sets (kd.num_edges ());
  for (unsigned i = 1; i <= kd.num_edges (); i ++)
    u_sets += u.find (i);
  
  ullmanset<1> sub_crossings (kd.n_crossings),
    sub_edges (kd.num_edges ());
  
  for (unsigned i = 1; i <= kd.num_edges (); i ++)
    {
      if (c % (u_sets.position (u.find (i)) + 1))
	sub_edges.push (i);
    }
  
  // sub edge x sublink edge
  unionfind<1> subu (sub_edges.card ());
  
  set<unsigned> active_comps;
  
  for (unsigned i = 1; i <= kd.n_crossings; i ++)
    {
      unsigned c1 = u_sets.position (u.find (kd.ept_edge (kd.crossings[i][1]))) + 1,
	c2 = u_sets.position (u.find (kd.ept_edge (kd.crossings[i][2]))) + 1;
      
      if (c % c1
	  && c % c2)
	{
	  sub_crossings.push (i);
	  active_comps += c1;
	  active_comps += c2;
	}
      else
	{
	  if (c % (u_sets.position (u.find (kd.ept_edge (kd.crossings[i][1]))) + 1))
	    {
	      subu.join (sub_edges.position (kd.ept_edge (kd.crossings[i][1])) + 1,
			 sub_edges.position (kd.ept_edge (kd.crossings[i][3])) + 1);
	    }
	  if (c % (u_sets.position (u.find (kd.ept_edge (kd.crossings[i][2]))) + 1))
	    {
	      subu.join (sub_edges.position (kd.ept_edge (kd.crossings[i][2])) + 1,
			 sub_edges.position (kd.ept_edge (kd.crossings[i][4])) + 1);
	    }
	}
    }
  
  n_crossings = (sub_crossings.card ()
		 + (c.card () - active_comps.card ()));
  
  assert (n_crossings > 0);
  
  ullmanset<1> subu_sets (sub_edges.card ());
  for (unsigned i = 1; i <= sub_edges.card (); i ++)
    subu_sets += subu.find (i);
  
  crossings = basedvector<basedvector<unsigned, 1>, 1> (n_crossings);
  for (unsigned i = 1; i <= n_crossings; i ++)
    crossings[i] = basedvector<unsigned, 1> (4);
  for (ullmanset_const_iter<1> i = sub_crossings; i; i ++)
    {
      unsigned c = i.val (),
	new_c = i.pos () + 1;
      
      unsigned e1 = (subu_sets.position
		     (subu.find (sub_edges.position (kd.ept_edge (kd.crossings[c][1])) + 1)) + 1),
	e2 = (subu_sets.position
	      (subu.find (sub_edges.position (kd.ept_edge (kd.crossings[c][2])) + 1)) + 1),
	e3 = (subu_sets.position
	      (subu.find (sub_edges.position (kd.ept_edge (kd.crossings[c][3])) + 1)) + 1),
	e4 = (subu_sets.position
	      (subu.find (sub_edges.position (kd.ept_edge (kd.crossings[c][4])) + 1)) + 1);
      
      if (kd.is_from_ept (kd.crossings[c][1]))
	crossings[new_c][1] = edge_from_ept (e1);
      else
	crossings[new_c][1] = edge_to_ept (e1);
      
      if (kd.is_from_ept (kd.crossings[c][2]))
	crossings[new_c][2] = edge_from_ept (e2);
      else
	crossings[new_c][2] = edge_to_ept (e2);

      if (kd.is_from_ept (kd.crossings[c][3]))
	crossings[new_c][3] = edge_from_ept (e3);
      else
	crossings[new_c][3] = edge_to_ept (e3);
      
      if (kd.is_from_ept (kd.crossings[c][4]))
	crossings[new_c][4] = edge_from_ept (e4);
      else
	crossings[new_c][4] = edge_to_ept (e4);
    }
  
  unsigned e = subu.num_sets ();
  unsigned new_c = sub_crossings.card ();
  for (smallbitset_const_iter i = c; i; i ++)
    {
      if (active_comps % i.val ())
	continue;
      
      unsigned e1 = (subu_sets.position
		     (subu.find (sub_edges.position (u_sets.nth (i.val () - 1)) + 1)) + 1);
      unsigned e2 = ++ e;
      unsigned c = ++ new_c;
      
      crossings[c][1] = edge_from_ept (e1);
      crossings[c][2] = edge_to_ept (e1);
      
      crossings[c][3] = edge_to_ept (e2);
      crossings[c][4] = edge_from_ept (e2);
    }
  assert (e == num_edges ());
  assert (new_c == n_crossings);
  
  // ?? break this out into aux function
  ept_crossing = basedvector<unsigned, 1> (num_epts ());
  ept_index = basedvector<unsigned, 1> (num_epts ());
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      for (unsigned j = 1; j <= 4; j ++)
	{
	  unsigned p = crossings[i][j];
	  ept_crossing[p] = i;
	  ept_index[p] = j;
	}
    }
  
#ifndef NDEBUG
  check_crossings ();
#endif
  
  calculate_smoothing_orientation ();
  calculate_nminus_nplus ();
}

knot_diagram::knot_diagram (disjoint_union,
			    const knot_diagram &kd1,
			    const knot_diagram &kd2)
  : name(kd1.name + "+" + kd2.name),
    n_crossings(kd1.n_crossings + kd2.n_crossings),
    marked_edge(0),
    crossings(n_crossings),
    nminus(kd1.nminus + kd2.nminus),
    nplus(kd1.nplus + kd2.nplus)
{
  assert (kd1.marked_edge == 0);
  assert (kd2.marked_edge == 0);
  
  for (unsigned i = 1; i <= n_crossings; i ++)
    crossings[i] = basedvector<unsigned, 1> (4);
  
  for (unsigned i = 1; i <= kd1.n_crossings; i ++)
    for (unsigned j = 1; j <= 4; j ++)
      crossings[i][j] = kd1.crossings[i][j];
  
  for (unsigned e = 1; e <= kd1.num_edges (); e ++)
    {
      if (kd1.edge_smoothing_oriented % e)
	edge_smoothing_oriented.push (e);
    }
  
  for (unsigned i = 1; i <= kd2.n_crossings; i ++)
    for (unsigned j = 1; j <= 4; j ++)
      crossings[kd1.n_crossings + i][j] = kd1.num_epts () + kd2.crossings[i][j];
  
  for (unsigned e = 1; e <= kd2.num_edges (); e ++)
    {
      if (kd2.edge_smoothing_oriented % e)
	edge_smoothing_oriented.push (kd1.num_edges () + e);
    }
  
  // ?? break this out into aux function
  ept_crossing = basedvector<unsigned, 1> (num_epts ());
  ept_index = basedvector<unsigned, 1> (num_epts ());
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      for (unsigned j = 1; j <= 4; j ++)
	{
	  unsigned p = crossings[i][j];
	  ept_crossing[p] = i;
	  ept_index[p] = j;
	}
    }
  
#ifndef NDEBUG
  check_crossings ();
#endif
}

knot_diagram::knot_diagram (mirror, const knot_diagram &kd)
  : name("mirror(" + kd.name + ")"),
    n_crossings(kd.n_crossings),
    marked_edge(0),
    crossings(n_crossings),
    ept_crossing(num_epts ()),
    ept_index(num_epts ()),
    nminus(0), nplus(0)
{
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      basedvector<unsigned, 1> v (4);
      v[1] = kd.crossings[i][1];
      v[2] = kd.crossings[i][4];
      v[3] = kd.crossings[i][3];
      v[4] = kd.crossings[i][2];
      crossings[i] = v;
    }
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      for (unsigned j = 1; j <= 4; j ++)
	{
	  unsigned c = crossings[i][j];
	  ept_crossing[c] = i;
	  ept_index[c] = j;
	}
    }
  
  calculate_smoothing_orientation ();
  calculate_nminus_nplus ();
  assert (nminus == kd.nplus);
  assert (nplus == kd.nminus);
}

void
knot_diagram::check_crossings ()
{
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      for (unsigned j = 1; j <= 4; j ++)
	{
	  unsigned p = crossings[i][j];
	  
	  assert (ept_crossing[p] == i);
	  assert (ept_index[p] == j);
	}
    }
  
  for (unsigned i = 1; i <= num_edges (); i ++)
    {
      unsigned to = edge_to_ept (i);
      assert (is_from_ept (crossings[ept_crossing[to]][add_base1_mod4 (ept_index[to], 2)]));
    }
}

void
knot_diagram::rotate_crossing (unsigned c)
{
  std::swap (crossings[c][1], crossings[c][3]);
  std::swap (crossings[c][2], crossings[c][4]);
  for (unsigned j = 1; j <= 4; j ++)
    {
      unsigned p = crossings[c][j];
      ept_index[p] = j;
    }
}

unsigned
knot_diagram::total_linking_number () const
{
  unionfind<1> u (num_edges ());
  
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      u.join (ept_edge (crossings[i][1]),
	      ept_edge (crossings[i][3]));
      u.join (ept_edge (crossings[i][2]),
	      ept_edge (crossings[i][4]));
    }
  unsigned n = u.num_sets ();
  
  map<unsigned, unsigned> root_comp;
  unsigned t = 0;
  for (unsigned i = 1; i <= num_edges (); i ++)
    {
      if (u.find (i) == i)
	{
	  ++ t;
	  root_comp.push (i, t);
	}
    }
  assert (t == n);
  
  unsigned total_lk = 0;
  
  for (unsigned i = 1; i <= n; i ++)
    for (unsigned j = i + 1; j <= n; j ++)
      {
	if (i == j)
	  continue;
	
	int lk = 0;
	for (unsigned x = 1; x <= n_crossings; x ++)
	  {
	    unsigned r1 = root_comp(u.find (ept_edge (crossings[x][1]))),
	      r2 = root_comp(u.find (ept_edge (crossings[x][2])));
	    if (((r1 == i) && (r2 == j))
		|| ((r2 == i) && (r1 == j)))
	      {	
		if (is_to_ept (crossings[x][1]) == is_to_ept (crossings[x][4]))
		  lk ++;
		else
		  lk --;
	      }
	  }
	assert (is_even (lk));
	lk /= 2;
	
	total_lk += abs (lk);
      }
  
  return total_lk;
}

knot_diagram::knot_diagram (connect_sum,
			    const knot_diagram &d1,
			    const knot_diagram &d2)
  : name(d1.name + "\\#" + d2.name),
    n_crossings(d1.n_crossings + d2.n_crossings),
    marked_edge(0),
    crossings(n_crossings),
    ept_crossing(num_epts ()),
    ept_index(num_epts ()),
    nminus(0), nplus(0)
{
  for (unsigned i = 1; i <= d1.n_crossings; i ++)
    crossings[i] = basedvector<unsigned, 1> (COPY, d1.crossings[i]);
  
  for (unsigned i = 1; i <= d2.n_crossings; i ++)
    {
      basedvector<unsigned, 1> v (4);
      for (unsigned j = 1; j <= 4; j ++)
	v[j] = d1.num_epts () + d2.crossings[i][j];
      crossings[d1.n_crossings + i] = v;
    }

  unsigned p1 = d1.edge_from_ept (1);
  crossings[d1.ept_crossing[p1]][d1.ept_index[p1]]
    = edge_from_ept (d1.num_edges () + 1);
  
  unsigned p2 = d2.edge_from_ept (1);
  crossings[d1.n_crossings + d2.ept_crossing[p2]][d2.ept_index[p2]]
    = d1.edge_from_ept (1);
  
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      for (unsigned j = 1; j <= 4; j ++)
	{
	  unsigned p = crossings[i][j];
	  ept_crossing[p] = i;
	  ept_index[p] = j;
	}
    }
  
#ifndef NDEBUG
  check_crossings ();
#endif
  
  orient ();
  calculate_smoothing_orientation ();
  calculate_nminus_nplus ();
  
}

knot_diagram::knot_diagram (const std::string &name_, unsigned n_crossings_, unsigned crossings_ar[][4])
  : name(name_),
    n_crossings(n_crossings_),
    marked_edge(0),
    crossings(n_crossings),
    ept_crossing(num_epts ()),
    ept_index(num_epts ()),
    nminus(0), nplus(0)
{
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      basedvector<unsigned, 1> v (4);
      v[1] = crossings_ar[i - 1][0];
      v[2] = crossings_ar[i - 1][1];
      v[3] = crossings_ar[i - 1][2];
      v[4] = crossings_ar[i - 1][3];
      crossings[i] = v;
      for (unsigned j = 1; j <= 4; j ++)
	{
	  unsigned p = crossings[i][j];
	  ept_crossing[p] = i;
	  ept_index[p] = j;
	}
    }
  
  calculate_smoothing_orientation ();
  calculate_nminus_nplus ();
}

knot_diagram::knot_diagram (const std::string &name_, const basedvector<basedvector<unsigned, 1>, 1> &crossings_)
  : name(name_),
    n_crossings(crossings_.size ()),
    marked_edge(0),
    crossings(crossings_),
    ept_crossing(num_epts ()),
    ept_index(num_epts ()),
    nminus(0), nplus(0)
{
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      for (unsigned j = 1; j <= 4; j ++)
	{
	  unsigned p = crossings[i][j];
	  ept_crossing[p] = i;
	  ept_index[p] = j;
	}
    }
  
  calculate_smoothing_orientation ();
  calculate_nminus_nplus ();
}

void
knot_diagram::orient ()
{
  bitset done (num_edges ());
  for (unsigned i = 1; i <= num_edges (); i ++)
    {
      if (done % i)
	continue;
      
      unsigned p = edge_to_ept (i);
      for (;;)
	{
	  unsigned e = ept_edge (p);
	  done.push (e);
	  if (is_from_ept (p))
	    {
	      unsigned e_from = edge_from_ept (e),
		e_to = edge_to_ept (e),
		c_from = ept_crossing[e_from],
		c_to = ept_crossing[e_to],
		j_from = ept_index[e_from],
		j_to = ept_index[e_to];
	      ept_crossing[e_to] = c_from;
	      ept_index[e_to] = j_from;
	      ept_crossing[e_from] = c_to;
	      ept_index[e_from] = j_to;
	      crossings[c_from][j_from] = e_to;
	      crossings[c_to][j_to] = e_from;
	      
	      p = e_to;
	    }
	  
	  p = crossings[ept_crossing[p]][add_base1_mod4 (ept_index[p], 2)];
	  if (ept_edge (p) == i)
	    break;
	  p = edge_other_ept (p);
	}
    }
  assert (done.card () == num_edges ());
  
#ifndef NDEBUG
  check_crossings ();
#endif
}

void
knot_diagram::calculate_nminus_nplus ()
{
  assert (nplus == 0 && nminus == 0);
  
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      if (is_to_ept (crossings[i][1]) == is_to_ept (crossings[i][4]))
	nplus ++;
      else
	nminus ++;
    }
}

void
knot_diagram::calculate_smoothing_orientation ()
{
  edge_smoothing_oriented = set<unsigned> ();
  
  set<unsigned> done;
  vector<unsigned> q;
  
  for (unsigned i = 1; i <= num_edges (); i ++)
    {
      if (done % i)
	continue;
      
      edge_smoothing_oriented.push (i);
      done.push (i);
      q.append (i);
      
      while (q.size () != 0)
	{
	  unsigned e = q.pop ();
	  assert (done % e);
	  
	  unsigned p = edge_smoothing_to_ept (e);
	  
	  unsigned r = resolve_next_ept (p, 0);
	  unsigned f = ept_edge (r);
	  if (done % f)
	    assert (is_smoothing_from_ept (r));
	  else
	    {
	      if (is_from_ept (r))
		edge_smoothing_oriented.push (f);
	      done.push (f);
	      q.append (f);
	    }
	  
	  unsigned r2 = resolve_next_ept (p, 1);
	  unsigned f2 = ept_edge (r2);
	  if (done % f2)
	    assert (is_smoothing_from_ept (r2));
	  else
	    {
	      if (is_from_ept (r2))
		edge_smoothing_oriented.push (f2);
	      done.push (f2);
	      q.append (f2);
	    }
	}
    }
  assert (done.card () == num_edges ());
}

unsigned
knot_diagram::resolve_next_ept (unsigned p, bool resolution) const
{
  static unsigned lookup[2][4] = { { 2, 1, 4, 3 }, { 4, 3, 2, 1 } };
  
  unsigned c = ept_crossing[p],
    i = ept_index[p];
  assert (i >= 1 && i <= 4);
  i = lookup[(int)resolution][i - 1];
  return crossings[c][i];
}

static unsigned crossing_index_corner (unsigned c, unsigned i)
{
  return (c - 1) * 4 + i;
}

static unsigned corner_crossing (unsigned x)
{
  return (x - 1) / 4 + 1;
}

static unsigned corner_index (unsigned x)
{
  return ((x - 1) % 4) + 1;
}

unsigned
knot_diagram::num_components () const
{
  unionfind<1> u (num_edges ());
  
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      u.join (ept_edge (crossings[i][1]),
	      ept_edge (crossings[i][3]));
      u.join (ept_edge (crossings[i][2]),
	      ept_edge (crossings[i][4]));
    }
  return u.num_sets ();
}

directed_multigraph
knot_diagram::black_graph (basedvector<unsigned, 1> &bg_edge_height) const
{
  unsigned n_corners = n_crossings * 4;
  
#if 1
  for (unsigned c = 1; c <= n_crossings; c ++)
    for (unsigned j = 1; j <= 4; j ++)
      {
	unsigned x = crossing_index_corner (c, j);
	assert (x >= 1 && x <= n_corners);
	
	assert (corner_crossing (x) == c);
	assert (corner_index (x) == j);
      }
  
  for (unsigned x = 1; x <= n_corners; x ++)
    {
      unsigned c = corner_crossing (x);
      assert (c >= 1 && c <= n_crossings);
      
      unsigned j = corner_index (x);
      assert (j >= 1 && j <= 4);
      
      assert (crossing_index_corner (c, j) == x);
    }
#endif
  
  unionfind<1> u (n_corners);
  
  unsigned n_edges = num_edges ();
  for (unsigned i = 1; i <= n_edges; i ++)
    {
      unsigned i_from = edge_from_ept (i),
	i_to = edge_to_ept (i);
      
      u.join (crossing_index_corner (ept_crossing (i_from),
				     ept_index (i_from)),
	      crossing_index_corner (ept_crossing (i_to),
				     add_base1_mod4 (ept_index (i_to), 3)));
      u.join (crossing_index_corner (ept_crossing (i_from),
				     add_base1_mod4 (ept_index (i_from), 3)),
	      crossing_index_corner (ept_crossing (i_to),
				     ept_index (i_to)));
    }
  
  ullmanset<1> present (n_corners);
  for (unsigned i = 1; i <= n_corners; i ++)
    present += u.find (i);
  
  basedvector<unsigned, 1> edge_from, edge_to;
  
  basedvector<unsigned, 1> edge_height;
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      edge_from.append (present.position (u.find (crossing_index_corner (i, 1))) + 1);
      edge_to.append (present.position (u.find (crossing_index_corner (i, 3))) + 1);
      edge_height.append (1);

      edge_from.append (present.position (u.find (crossing_index_corner (i, 2))) + 1);
      edge_to.append (present.position (u.find (crossing_index_corner (i, 4))) + 1);
      edge_height.append (0);
    }
  
  assert (u.num_sets () == present.card ());
  directed_multigraph bwg (u.num_sets (), edge_from, edge_to);
  // display ("bwg:\n", bwg);
  
  assert (bwg.num_components () == 2);
  
  basedvector<unsigned, 1> edge_inj,
    vertex_inj;
  
  directed_multigraph bg = bwg.component (1, edge_inj, vertex_inj);
  
  bg_edge_height = basedvector<unsigned, 1> (bg.num_edges ());
  for (unsigned i = 1; i <= bg.num_edges (); i ++)
    bg_edge_height[i] = edge_height[edge_inj[i]];
  
  return bg;
}

basedvector<basedvector<int, 1>, 1> 
knot_diagram::planar_diagram_crossings () const
{
  unsigned n_edges = num_edges ();
  
  ullmanset<1> edges (n_edges);
  for (unsigned e = 1; e <= n_edges; e ++)
    {
      if (edges % e)
	continue;
      
      for (unsigned i = e;;)
	{
	  edges.push (i);
	  
	  unsigned c = ept_crossing[edge_to_ept (i)],
	    j = ept_index[edge_to_ept (i)];
	  unsigned j2 = add_base1_mod4 (j, 2);
	  
	  assert (is_from_ept (crossings[c][j2]));
	  
	  i = ept_edge (crossings[c][j2]);
	  if (i == e)
	    break;
	}
    }
  assert (edges.card () == n_edges);
  
  basedvector<basedvector<int, 1>, 1> r (n_crossings);
  unsigned k = 0;
  
  bool first = 1;
  for (unsigned i = 1; i <= n_edges; i ++)
    {
      unsigned e = edges.nth (i - 1);
      
      unsigned c = ept_crossing[edge_to_ept (e)],
	j = ept_index[edge_to_ept (e)];
      if (j == 1)
	{
	  basedvector<int, 1> v (4);
	  v[1] = edges.position (ept_edge (crossings[c][1])) + 1;
	  v[2] = edges.position (ept_edge (crossings[c][2])) + 1;
	  v[3] = edges.position (ept_edge (crossings[c][3])) + 1;
	  v[4] = edges.position (ept_edge (crossings[c][4])) + 1;
	  r[++ k] = v;
	}
      else if (j == 3)
	{
	  basedvector<int, 1> v (4);
	  v[1] = edges.position (ept_edge (crossings[c][3])) + 1;
	  v[2] = edges.position (ept_edge (crossings[c][4])) + 1;
	  v[3] = edges.position (ept_edge (crossings[c][1])) + 1;
	  v[4] = edges.position (ept_edge (crossings[c][2])) + 1;
	  r[++ k] = v;
	}
      unsigned j2 = add_base1_mod4 (j, 2);
      assert (is_from_ept (crossings[c][j2]));
      
      unsigned e2 = ept_edge (crossings[c][j2]);
      unsigned i2 = edges.position (e2) + 1;
#if 0
      assert ((i == n_edges && i2 == 1)
	      || i2 == i + 1);
#endif
    }
  assert (k == n_crossings);
  
  return r;
}

basedvector<basedvector<int, 1>, 1>
knot_diagram::as_gauss_code () const
{
  set<unsigned> visited;
  unsigned m = num_components ();
  
  basedvector<basedvector<int, 1>, 1> gc (m);
  
  unsigned k = 0;  // index component
  for (unsigned i = 1; i <= num_edges (); i ++)
    {
      if (visited % i)
    continue;
      
      basedvector<int, 1> comp_gc;
      
      unsigned p = edge_to_ept (i);
      for (unsigned j = i;;)
    {
      visited.push (j);
      
      unsigned c = ept_crossing[p];
      
      int t = (is_over_ept (p)
           ? -c
           : c);
      comp_gc.append (t);
      
      p = crossings[c][add_base1_mod4 (ept_index[p], 2)];
      p = edge_other_ept (p);
      assert (is_to_ept (p));
      
      j = ept_edge (p);
      
      if (j == i)
        break;
    }
      
      ++ k;
      gc[k] = comp_gc;
    }
  assert (visited.card () == num_edges ());

#ifndef NDEBUG
  unsigned n = 0;
  for (unsigned i = 1; i <= m; i ++)
    n += gc[i].size ();
  assert (n == 2*n_crossings);
#endif
  
  return gc;
}

hash_t
knot_diagram::hash_self () const
{
  // ??? we can do better
  unsigned n_loops = 0;
  for (unsigned i = 1; i <= num_edges (); i ++)
    {
      if (ept_edge (edge_from_ept (i)) == ept_edge (edge_to_ept (i)))
	n_loops ++;
    }
  
  return hash_combine (hash (n_crossings),
		       hash (n_loops));
}

void
knot_diagram::show_ept (unsigned p) const
{
  unsigned e = ept_edge (p);
  if (is_to_ept (p))
    printf (">");
  printf ("%d", e);
  if (is_from_ept (p))
    printf (">");
}

void
knot_diagram::show_self () const
{
  printf ("knot_diagram %s", name.c_str ());
}

void
knot_diagram::display_self () const
{
  comment (); printf ("knot_diagram %s\n", name.c_str ());
  comment (); printf ("n_crossings = %d, n+ = %d, n- = %d\n", n_crossings, nplus, nminus);
  if (marked_edge)
    {
      comment ();
      printf ("marked_edge = %d\n", marked_edge);
    }
  
  comment ();
  for (unsigned i = 1; i <= crossings.size (); i ++)
    {
      printf ("%d:(", i);
      show_ept (crossings[i][1]);
      printf (" ");
      show_ept (crossings[i][2]);
      printf (" ");
      show_ept (crossings[i][3]);
      printf (" ");
      show_ept (crossings[i][4]);
      printf (")");
   }
  printf ("\n");
}
