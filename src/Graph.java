package com.udel.algorithms;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;
import java.util.Scanner;

class Neighbour {
	private int vertext;
	private Neighbour nextNeighbour;
	public Neighbour(int vertext, Neighbour nextNeighbour) {
		this.vertext = vertext;
		this.nextNeighbour = nextNeighbour;
	}
	public void setVertext(int vertext) {
		this.vertext = vertext;
	}
	public int getVertext() {
		return vertext;
	}
	public void setNextNeighbor(Neighbour nextNeighbour) {
		this.nextNeighbour = nextNeighbour;
	}
	public Neighbour getNextNeighbor() {
		return nextNeighbour;
	}
}

class Vertex {
	private String name;
	private Neighbour adjList;
	private Vertex parent;
	private String color;
	private boolean ignore;
	public boolean isIgnore() {
		return ignore;
	}
	public void setIgnore(boolean ignore) {
		this.ignore = ignore;
	}
	public Vertex(String name, Neighbour adjList) {
		this.name = name;
		this.adjList = adjList;
		this.ignore = false;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public Neighbour getAdjList() {
		return adjList;
	}
	public void setAdjList(Neighbour adjList) {
		this.adjList = adjList;
	}
	public Vertex getParent() {
		return parent;
	}
	public void setParent(Vertex parent) {
		this.parent = parent;
	}
	public String getColor() {
		return color;
	}
	public void setColor(String color) {
		this.color = color;
	}
}

class Graph {
	private ArrayList<Vertex> vertices =  new ArrayList<Vertex>();
	
	/**
	 * Default constructor. Reads from stdin.
	 * @throws IOException
	 */
	public Graph() throws IOException {
		this.buildGraphByStdin();
	}
	
	/**
	 * Alternative constructor. Reads from a file.
	 * @param fileName
	 * @throws FileNotFoundException
	 */
	public Graph(String fileName) throws FileNotFoundException {
		this.buildGraphByFile(fileName);
	}
	
	/**
	 * Builds the graph using input from stdin.
	 * @throws IOException
	 */
	private void buildGraphByStdin() throws IOException {
		InputStreamReader in= new InputStreamReader(System.in);
        BufferedReader input = new BufferedReader(in);
        String line;
		while ((line = input.readLine()) != null) {
			this.parseLine(line);
		}
	}
	
	/**
	 * Builds the graph using the input from a file.
	 * @param fileName
	 * @throws FileNotFoundException
	 */
	private void buildGraphByFile(String fileName) throws FileNotFoundException {
		Scanner fileScanner  = new Scanner(new File(fileName));
		while (fileScanner.hasNextLine()) {
			String line = fileScanner.nextLine();
			this.parseLine(line);
		}
		fileScanner.close();
	}
	
	/**
	 * Parses a line and add the elements to the graph.
	 * @param line
	 */
	private void parseLine(String line) {
		String tempArray[] = line.split("[:,]+");
		
		// check if the parent already exists in the list if not create one
		int parentIndex = findVertexIndexByName(tempArray[0]);
		Vertex parentVertex;
		if (parentIndex!=-1) {
			// this vertex already exists
			parentVertex = vertices.get(parentIndex);
		} else {
			parentVertex = new Vertex(tempArray[0].trim(), null);
			vertices.add(parentVertex);
			parentIndex = vertices.indexOf(parentVertex);
		}
	
		for (int i = 1; i < tempArray.length; i++) {
			int indexOfVertex = findVertexIndexByName(tempArray[i]);
			
			if (indexOfVertex != -1) {
				// vertex exists in the array => link the reference
			    // iterate through the adjList of the vertex and add the neighbor at the end
				Neighbour temp = parentVertex.getAdjList();
				if (temp == null) {
					parentVertex.setAdjList(new Neighbour(indexOfVertex, null));
				} else {
					while (temp.getNextNeighbor() != null) {
						temp = temp.getNextNeighbor();
					}
					temp.setNextNeighbor(new Neighbour(indexOfVertex, null));
				}
			} else {
				// create a new vertex, add it to the array list and link the reference
				Vertex childVertex = new Vertex(tempArray[i].trim(), null);
				vertices.add(childVertex);
				
				int indexOfChild = vertices.indexOf(childVertex);
				Neighbour temp = parentVertex.getAdjList();
				if (temp == null) {
					parentVertex.setAdjList(new Neighbour(indexOfChild, null));
				} else {
					while (temp.getNextNeighbor() != null) {
						temp = temp.getNextNeighbor();
					}
					temp.setNextNeighbor(new Neighbour(indexOfChild, null));
				}
			}
		}
	}
	
	/**
	 * Search for a vertex by name.
	 * @param name
	 * @return int The vertex's index or -1 if not found
	 */
	public int findVertexIndexByName(String name) {
		for (int i = 0; i < vertices.size(); i++) {
			if (vertices.get(i).getName().equalsIgnoreCase(name.trim())) {
				return i;
			}
		}
		return -1;
	}
	
	/**
	 * Finds subspecies of a species of certain order.
	 * @param number
	 * @param species
	 * @param order
	 * @return ArrayList<Vertex>
	 */
	public ArrayList<Vertex> cite(int number, String species, int order) {
		int index = this.findVertexIndexByName(species);
		if (index == -1) {
			return null;
		}
		Vertex v = vertices.get(index);
		ArrayList<Vertex> list =  new ArrayList<Vertex>();
		this.cite(list, v, number, order-1);
		return list;
	}
	
	/**
	 * Finds all subspecies of a species of certain order.
	 * @param species
	 * @param order
	 * @return ArrayList<Vertex>
	 */
	public ArrayList<Vertex> citeAll(String species, int order) {
		int index = this.findVertexIndexByName(species);
		if (index == -1) {
			return null;
		}
		ArrayList<Vertex> list =  new ArrayList<Vertex>();
		Vertex v = vertices.get(index);
		this.cite(list, v, -1, order-1);
		return list;
	}
	
	/**
	 * Recursive method for citing.
	 * @param list
	 * @param v
	 * @param number
	 * @param order
	 * @return int Remaining number of subspecies to print.
	 */
	private int cite(ArrayList<Vertex> list, Vertex v, int number, int order) {
		if (number == 0) {
			return number;
		}
		if (order <= 0) {
			// Reached provided order => print
			Neighbour nbr = v.getAdjList();
			while (number != 0 && nbr != null) {
				list.add(vertices.get(nbr.getVertext()));
				nbr = nbr.getNextNeighbor();
				number--;
			}
		} else {
			// Did not reach provided order => keep going
			for (Neighbour nbr = v.getAdjList(); nbr != null; nbr = nbr.getNextNeighbor()) {
				number = cite(list, vertices.get(nbr.getVertext()), number, order-1);
			}
		}
		return number;
	}
	
	/**
	 * Finds the most diverse species (the one that has more subspecies).
	 * @return Vertex
	 */
	public Vertex mostDiverse() {
		int max = -1;
		Vertex v = null;
		for (int i = 0; i < vertices.size(); i++) {
			int count = 0;
			for (Neighbour nbr = vertices.get(i).getAdjList(); nbr != null; nbr = nbr.getNextNeighbor()) {
				count++;
			}
			if (count > max) {
				max = count;
				v = vertices.get(i);
			}
		}
		return v;
	}

	/**
	 * Finds the lowest common ancestor of s2 and s3 starting from s1.
	 * @param species1
	 * @param species2
	 * @param species3
	 * @return Vertex
	 */
	public Vertex lowestCommonAncestor(String species1, String species2, String species3) {
		int v1i = this.findVertexIndexByName(species1);
		int v2i = this.findVertexIndexByName(species2);
		int v3i = this.findVertexIndexByName(species3);
		Vertex s1 = vertices.get(v1i);
		Vertex s2 = vertices.get(v2i);
		Vertex s3 = vertices.get(v3i);
		if (v1i < 0 || v2i < 0 || v3i < 0) {
			return null;
		} else if (species1.equalsIgnoreCase(species2) || species1.equalsIgnoreCase(species3)) {
			return null;
		} else {
			BFS(s1, null); // explores the entire graph to set the parent attribute of the vertexes
			return getCommonAncestor(s1, s2, s3);
		}
	}
	
	/**
	 * Recursive method for finding the lowest common ancestor.
	 * @param s1
	 * @param s2
	 * @param s3
	 * @return Vertex
	 */
	private Vertex getCommonAncestor(Vertex s1, Vertex s2, Vertex s3) {
		// While s1 is not reached
		if (s2.getName() != s1.getName()) {
			Vertex k = s2.getParent(); // Parent of last vertex
			s2.setIgnore(true); // Remove the edge connecting k-s2 (since we know s3 is not in its children)
			if (BFS(k, s3)) {
				// Found s3 in the children of k
				return k;
			} else {
				// Keep going
				return getCommonAncestor(s1, k, s3);
			}
		} else {
			return null;
		}
	}
	
	/**
	 * BFS for finding a specified vertex from a specified source.
	 * @param s
	 * @param w
	 * @return boolean
	 */
	public boolean BFS(Vertex s, Vertex w){
		Queue<Vertex> queue = new LinkedList<Vertex>();
		initializeVertices(w == null);
		s.setColor("GRAY");
		queue.add(s);
		while (!queue.isEmpty()) {
			Vertex u = queue.poll();
			for (Neighbour nbr = u.getAdjList(); nbr != null; nbr = nbr.getNextNeighbor()) {
				Vertex child = vertices.get(nbr.getVertext());
				if (!child.getColor().equalsIgnoreCase("GRAY") && !child.isIgnore()) {
					if (w != null && child.getName().equalsIgnoreCase(w.getName())) {
						// when w is not provided it will explore the entire graph.
						return true;
					} else {
						child.setColor("GRAY");
						child.setParent(u);
						queue.add(child);
					}
				}
			}
			u.setColor("BLACK");
		}
		return false;
	}
	
	/**
	 * Initializes vertices for BFS.
	 * @param firstTime if it is the first time that the graph is being initialized
	 */
	private void initializeVertices(boolean firstTime) {
		for (int i = 0; i < vertices.size(); i++) {
			vertices.get(i).setColor("WHITE");
			if (firstTime) {
				vertices.get(i).setIgnore(false);
				vertices.get(i).setParent(null);
			}
		}
	}

	/**
	 * Prints the graph.
	 */
	public void printGraph() {
		for (int i = 0; i < vertices.size(); i++) {
			System.out.print(vertices.get(i).getName());
			for (Neighbour nbr = vertices.get(i).getAdjList(); nbr != null; nbr = nbr.getNextNeighbor()) {
				System.out.print(" --> "+ vertices.get(nbr.getVertext()).getName());
			}
			System.out.println("\n");
		}
	}
	
	public static void main(String[] args) throws IOException {
		Graph graph;
		try {
			graph = new Graph("hyponymy.txt");
		} catch (FileNotFoundException e) {
			graph = new Graph();
		}
		
		Vertex v;
		ArrayList<Vertex> list;
		 
		//graph.printGraph();
		 
		v = graph.mostDiverse();
		System.out.println("Most diverse: " + v.getName());
		
		System.out.print("\n");
		
		System.out.println("3 subspecies of vertebrates of order 1:");
		list = graph.cite(3, "vertebrates", 1);
		for (int i = 0; i < list.size(); i++) {
			System.out.print(list.get(i).getName() + ", ");
		}
		
		System.out.print("\n\n");
		
		System.out.println("4 subspecies of vertebrates of order 2:");
		list = graph.cite(4, "vertebrates", 2);
		for (int i = 0; i < list.size(); i++) {
			System.out.print(list.get(i).getName() + ", ");
		}
		
		System.out.print("\n\n");
		
		System.out.println("11 subspecies of carnivores of order 2:");
		list = graph.cite(11, "carnivore", 2);
		for (int i = 0; i < list.size(); i++) {
			System.out.print(list.get(i).getName() + ", ");
		}
		
		System.out.print("\n\n");
		
		System.out.println("15 subspecies of pet of order 3:");
		list = graph.cite(15, "pet", 3);
		for (int i = 0; i < list.size(); i++) {
			System.out.print(list.get(i).getName() + ", ");
		}
		
		System.out.print("\n\n");
		
		System.out.println("All subspecies of vertebrates of order 2:");
		list = graph.citeAll("vertebrates", 2);
		for (int i = 0; i < list.size(); i++) {
			System.out.print(list.get(i).getName() + ", ");
		}
		
		System.out.print("\n\n");
		
		v = graph.lowestCommonAncestor("animalia", "felidae", "canidae");
		System.out.println("Lowest common ancestor of felidae and canidae from animalia: " + v.getName());
		v = graph.lowestCommonAncestor("pet", "fish", "cat");
		System.out.println("Lowest common ancestor of fish and cat from pet: " + v.getName());
		v = graph.lowestCommonAncestor("Entity", "water", "pot");
		System.out.println("Lowest common ancestor of water and pot from entity: " + v.getName());
	}
}