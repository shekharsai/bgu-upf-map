1. Original domains, their compiled versions possible using the code and their modified versions 
needed for the experiments (modifications contain mutex relations etc), and problems used in the 
experiments are placed in test-domains.

2. Same version of the code is applicable for Rep 1: well-defined and Rep 2: well-formed cases and 
is availbale in the folder name pddl-parser-multiagent.

3. For Rep 3 the relevant code is available in the folder name conc-aij-multiagent. 

4. Code for Rep 3 still contains some old code-snippet used for Rep 1, but is not in use. I will 
remove it once I get time.

6. To run the code: 

	(a) Compile and build: scons examples/serialize_cn/
	
	(b) Translation: examples/serialize_cn/serialize.bin .../domain-maze.pddl .../P05.pddl 
																		> dom.pddl 2>ins.pddl
																		
	(c) Run using FD: ./fast-downward.py .../dom.pddl .../ins.pddl --heuristic h="ff()" 
														--search "lazy_greedy(h, preferred=h)"
														
5. Reach me at: shashankshekhar2010@gmail.com
