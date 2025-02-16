#include<bits/stdc++.h>

class functionalGraph{
	public:
		int n;
		int cycle_count;
		std::vector<int> to;
		std::vector<std::vector<int>> rev_g;        // odwrotny graf pomaga na dfs po drzewach.
		std::vector<std::vector<int>> cycle_nodes;  // konkretne wieszchołki w i-tym cyckly
		std::vector<int> cycle_tag;    						  // { - 1 jeśli nie jest w cycklu inaczej jakieś tag }
		std::vector<int> cycle_siz;     					  // cycle_siz[i] = { rozmiar i-tego cycklu } NIE OD WIESZCHOŁKU TRZEBA cycle_siz[cycle_tag[v]]
		std::vector<int> in_cycle;      					  // in_cycle[i]  = { 1 jeśli jest w cycklu, 0 inaczej }
		std::vector<int> root;          					  // czyli gdzie zaczyna się cykl lub po prostu korzeń drzewa.
		std::vector<int> depth;         					  // głebokość wieszchołków 
		std::vector<int> index_in_cycle;            // gdzie się znajdują w tablicy cyklów lub -1 jeśli nie jest w cyklu.
		std::vector<std::vector<int>> go;
		std::vector<int> sub_siz;
		std::vector<int> in;
		std::vector<int> out;
		int LG;
		
		functionalGraph (int _n, std::vector<int> _to) : n(_n){
			to = _to;
			cycle_count = 0;
			assert((int)to.size() == n);
			rev_g.resize(n);
			for(int i = 0;i < n;i++){
				rev_g[to[i]].push_back(i);
			}
			cycle_tag.assign(n, -1); // -1 że nie jest w cyclu
			root.resize(n);
			cycle_siz.resize(n);
			cycle_nodes.resize(n);
			depth.resize(n);
			in_cycle.resize(n);
			go.resize(n);
			sub_siz.resize(n);
			in.resize(n);
			out.resize(n);
			index_in_cycle.assign(n, -1);
			LG = 1;
			while((1 << LG) <= n) LG++;
			LG += 1;
		}
		
		void build(){ // O(n)
			find_cycles();
			find_trees();
		}
		
		void build_jumps(){ // O(NlogN)
			for(int i = 0;i < n;i++){
				go[i].resize(LG);
				go[i][0] = to[i];
			}
			for(int l = 1;l < LG;l++){
				for(int i = 0;i < n;i++){
					go[i][l] = go[ go[i][l - 1] ][l - 1];
				}
			}
		}
		
		void find_cycles(){
			// oblicza czy wieszchołek jest w cyklu
			// jesli tak to przyporządkowuje etykiete
			// do którego, i zapisuje cykle według wierzchołków.
			std::vector<int> vis(n);
			int runda = 0;
			for(int i = 0;i < n;i++){
				if(vis[i]) continue;
				int x = i;
				runda += 1;
				do {
					vis[x] = runda;
					x = to[x];
				} while(!vis[x]);
				if(vis[x] == runda){ // to mamy cykl
					std::vector<int> c;
					int y = x;
					do {
						c.push_back(y);
						y = to[y];
					} while(y != x);
					cycle_siz[cycle_count] = (int)c.size();
					int _i = 0;
					for(int z : c){
						in_cycle[z] = 1;
						cycle_tag[z] = cycle_count;
						index_in_cycle[z] = _i++;
					}
					cycle_nodes[cycle_count] = c; // zapis wieszchołków.
					cycle_count += 1;
				}
			}
		}

		void find_trees(){
			int time = 0;
			auto dfs_down = [&](auto &&dfs_down, int u) -> void{
				sub_siz[u] = 1;
				in[u] = time++;
				for(int v : rev_g[u]) if(!in_cycle[v]){
					// nie sprawdzamy czy ojciec ponieważ drzewo skierowane.
					depth[v] = depth[u] + 1;
					root[v] = root[u];
					dfs_down(dfs_down, v);
					sub_siz[u] += sub_siz[v];
				}
				out[u] = time++;
			};
			
			for(int i = 0;i < n;i++){
				if(in_cycle[i]){
					root[i] = i;
					dfs_down(dfs_down, i);
				}
			}
			
		}
		
		int jump(int v, long long k){
			// chcemy wiedziec gdzie będziemy po k krokach.
			// O(log(N)), jak się ktoś postara to da się w O(1).
			for(int l = LG - 1;l >= 0;l--){
				if((1 << l) <= k){
					v = go[v][l];
					k -= (1 << l);
				}
			}
			assert(k == 0 || in_cycle[v]);
			if(k == 0)
				return v;
			k %= cycle_siz[cycle_tag[v]];
			for(int l = LG - 1;l >= 0;l--){
				if((1 << l) <= k){
					v = go[v][l];
					k -= (1 << l);
				}
			}
			return v;
		}
		
		bool isAnc(int a, int b){ // isAnc(a, a) returns true!!
			return in[a] <= in[b] && out[b] <= out[a];
		}
		
		int distCycle(int a, int b){ // from a to b it matter
			assert(cycle_tag[a] != -1 && cycle_tag[b] != -1);
			assert(cycle_tag[a] == cycle_tag[b]);
			if(index_in_cycle[a] <= index_in_cycle[b]) return index_in_cycle[b] - index_in_cycle[a];
			return index_in_cycle[b] + cycle_siz[cycle_tag[a]] - index_in_cycle[a];
		}
		
		int dist(int a, int b){ // O(1).
			// ile skoków od a żeby doiśc do b lub -1 jeśli się nie da.
			// 3 przypadki żeby w ogóle była jakaś szansa że można
			// 1) to samo drzewo i b jest przodkiem.
			// 2) ten sam cykl.
			// 3) a jest w drzewie i korzen a jest w cyklu z b.
			if(root[a] == root[b]){     // to samo drzewo
  			if(isAnc(b, a)) return depth[a] - depth[b];
				return -1;
			}
			if(cycle_tag[a] == cycle_tag[b] && cycle_tag[a] != -1){ // ten sam cykl
				return distCycle(a, b);
			}
			int res = depth[a];
			a = root[a];
			if(cycle_tag[a] != cycle_tag[b]) return -1;
			res += distCycle(a, b);
			return res;
		}
};
