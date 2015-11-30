
#include <string.h>

#define TRUE 1
#define FALSE 0
#define limit 0

//groupings
#define v        'aeiouy'
#define v_WXY    v + 'wxY'   // v with 'w', 'x' and 'y'-consonant
#define v_LSZ    v + 'lsz'   // v with 'l', 's', 'z'

static int p1; // position past m>0
static int p2; // position past m>1

static int Y_found; // possible values are 0 or 1

static char *b; // buffer to hold the string under stemming process
static int k,k0,j;

//functions

/* cons(i) is TRUE <=> b[i] is a consonant. */

static int cons(int i)
{  switch (b[i])
   {  case 'a': case 'e': case 'i': case 'o': case 'u': return FALSE;
      case 'y': return (i==k0) ? TRUE : !cons(i-1);
      default: return TRUE;
   }
}

static int isVowel(int i){
	 switch (b[i])
	   {  case 'a': case 'e': case 'i': case 'o': case 'u': return TRUE;
	      case 'y': return (i==k0) ? FALSE : cons(i-1);
	      default: return FALSE;
	   }
}

static int isPreceededByVowel(int i){

	for (int p=0;p<=i;p++){
		 switch (b[p])
		   {  case 'a': case 'e': case 'i': case 'o': case 'u': return TRUE;
		      case 'y': {
		    	  if(p==k0){

		    	  }else if(cons(p-1)){
		    		  return TRUE;
		    	  }
		      }
		   }
	}
	return FALSE;
}

static int isShortVowelAt(int p){
	 // if (out_grouping_b(b, g_v_WXY, 89, 121, 0)) return 0;
	 //   if (in_grouping_b(b, g_v, 97, 121, 0)) return 0;
	 //   if (out_grouping_b(b, g_v, 97, 121, 0)) return 0;

	  if(b[p]=='w' || b[p]=='x' || b[p]=='y'){
		  return FALSE;
	  }
	    return TRUE;
}


/* ends(s) is TRUE <=> k0,...k ends with the string s. */

static int ends(char * s)
{  int length = s[0];
   if (s[length] != b[k]) return FALSE; /* tiny speed-up */
   if (length > k-k0+1) return FALSE;
   if (memcmp(b+k-length+1,s+1,length) != 0) return FALSE;
   j = k-length;
   return TRUE;
}

/* setto(s) sets (j+1),...k to the characters in the string s, readjusting
   k. */

static void setto(char * s)
{  int length = s[0];
   memmove(b+j+1,s+1,length);
   k = j+length;
}

static int R1(int cursor){

  if(p1!=limit && !(cursor<=0) && p1<=cursor){
	  return TRUE;
  }
	return FALSE;
}

static int R2(int cursor){

	  if(p2!=limit && !(cursor<=0) && p2<=cursor){
		  return TRUE;
	  }
		return FALSE;
}

static void setMarkers(){
	for (int p=0;p<=k;p++){
		if(isVowel(p) && cons(p+1)){
			p1=p;
			break;
		}
	}

	for(int q=p1+1;q<=k;q++){
					if(isVowel(q) && cons(q+1)){
						p2=q;
						break;
					}
	}
}

/*
 * define Step_1a as (
        [substring] among (
            'sses' (<-'ss')
            'ies'  (<-'i')
            'ss'   ()
            's'    (delete)
        )
    )
 * */

static void Step_1a() {

	if (b[k] == 's') {
		if (ends("\04" "sses")) {
			k -= 2;
		} else if (ends("\03" "ies")) {
			setto("\01" "i");
			printf("%s",b);
		} else if(b[k-1] != 's') {
			k--;
		}
	}
}

/*  define Step_1b as (
        [substring] among (
            'eed'  (R1 <-'ee')
            'ed'
            'ing' (
                test gopast v  delete
                test substring among(
                    'at' 'bl' 'iz'
                         (<+ 'e')
                    'bb' 'dd' 'ff' 'gg' 'mm' 'nn' 'pp' 'rr' 'tt'
                    // ignoring double c, h, j, k, q, v, w, and x
                         ([next]  delete)
                    ''   (atmark p1  test shortv  <+ 'e')
                )
            )
        )
    )*/
static void Step_1b(){

	if (ends("\03" "eed")) {
		int temp = k;
		if(R1(temp-3)){
              k--;
		}
	}else if (ends("\02" "ed")) {
		int temp = k;
		 if(isPreceededByVowel(temp-3)){
		  k -=2;
		        if(ends("\02" "at") || ends("\02" "bl") || ends("\02" "iz")){
		             	j +=2;
		              setto("\01" "e");
		          }
		 }
	}else if (ends("\03" "ing")) {
		int temp = k;
		// test gopast v  delete
        if(isPreceededByVowel(temp-3)){ // ing are removed if they are preceded by a vowel. commented this as this is against converting conflating to conflate
        	k -=3;
        	if(ends("\02" "at") || ends("\02" "bl") || ends("\02" "iz")){
        		j +=2;
        		setto("\01" "e");
        	 }else if(ends("\02" "bb") || ends("\02" "ff") || ends("\02" "gg")||ends("\02" "mm") || ends("\02" "nn") || ends("\02" "pp")||ends("\02" "rr") || ends("\02" "tt")){
        		k--;
        		if(isShortVowelAt(p1)){
        			j++;
        			setto("\01" "e");
        		}
        	 }
        }
	}
}

/*  define Step_1c as (
        ['y' or 'Y']
        gopast v
        <-'i'
    )
    */

static void Step_1c(){
	int temp = k;
	if(ends("\01" "y") && isPreceededByVowel(temp-k)){
       setto("\01" "i");
	}
}

/*
 *     define Step_2 as (
        [substring] R1 among (
            'tional'  (<-'tion')
            'enci'    (<-'ence')
            'anci'    (<-'ance')
            'abli'    (<-'able')
            'entli'   (<-'ent')
            'eli'     (<-'e')
            'izer' 'ization'
                      (<-'ize')
            'ational' 'ation' 'ator'
                      (<-'ate')
            'alli'    (<-'al')
            'alism' 'aliti'
                      (<-'al')
            'fulness' (<-'ful')
            'ousli' 'ousness'
                      (<-'ous')
            'iveness' 'iviti'
                      (<-'ive')
            'biliti'  (<-'ble')
        )
    )
 * */

static void Step_2(){

	if(R1(k-6) && ends("\06" "tional")){
        setto("\04" "tion");
	}else if(R1(k-4) && ends("\04" "enci")){
        setto("\04" "ence");
	}else if(R1(k-4) && ends("\04" "anci")){
        setto("\04" "ance");
	}else if(R1(k-5) && ends("\05" "entli")){
        setto("\03" "ent");
	}else if(R1(k-3) && ends("\03" "eli")){
        setto("\01" "e");
	}else if((R1(k-4) && ends("\04" "izer") ||(R1(k-7) && ends("\07" "ization")))){
        setto("\03" "ize");
	}else if((R1(k-7) && ends("\07" "ational") ||(R1(k-5) && ends("\05" "ation"))) || (R1(k-4) && ends("\04" "ator"))){
        setto("\03" "ate");
	}else if(R1(k-2) && ends("\04" "alli")){
        setto("\02" "al");
	}else if(R1(k-7) && ends("\07" "fulness")){
        setto("\03" "ful");
	}else if((R1(k-5) && ends("\05" "ousli") ||(R1(k-7) && ends("\07" "ousness")))){
        setto("\03" "ous");
	}else if((R1(k-7) && ends("\07" "iveness") ||(R1(k-5) && ends("\05" "iviti")))){
        setto("\03" "ive");
	}else if(R1(k-6) && ends("\06" "biliti")){
        setto("\03" "ble");
	}

}

/* define Step_3 as (
        [substring] R1 among (
            'alize'   (<-'al')
            'icate' 'iciti' 'ical'
                      (<-'ic')
            'ative' 'ful' 'ness'
                      (delete)
        )
    )*/

static void Step_3(){
	if(R1(k-5) && ends("\05" "alize")){
	        setto("\02" "al");
		}else if((R1(k-5) && ends("\05" "icate") ||(R1(k-5) && ends("\05" "iciti"))) || (R1(k-4) && ends("\04" "ical"))){
	        setto("\02" "ic");
		}else if(R1(k-5) && ends("\05" "ative")){
             k -=5;
		}else if(R1(k-3) && ends("\03" "ful")){
			  k -=3;
		}else if(R1(k-4) && ends("\04" "ness")){
			  k -=4;
		}
}

/*define Step_4 as (
        [substring] R2 among (
            'al' 'ance' 'ence' 'er' 'ic' 'able' 'ible' 'ant' 'ement'
            'ment' 'ent' 'ou' 'ism' 'ate' 'iti' 'ous' 'ive' 'ize'
                      (delete)
            'ion'     ('s' or 't' delete)
        )
    )
 * */
static void Step_4(){

	if(R2(k-2) && ends("\02" "al")){
		  k -=2;
	}else if(R2(k-4) && ends("\04" "ance")){
		  k -=4;
    }else if(R2(k-4) && ends("\04" "ence")){
			  k -=4;
	}else if(R2(k-2) && ends("\02" "er")){
		  k -=2;
	}else if(R2(k-2) && ends("\02" "ic")){
		  k -=2;
	}else if(R2(k-4) && ends("\04" "able")){
		  k -=4;
	}else if(R2(k-4) && ends("\04" "ible")){
		  k -=4;
	}else if(R2(k-3) && ends("\03" "ant")){
		  k -=3;
	}else if(R1(k-5) && ends("\05" "ement")){
		  k -=5;
	}else if(R2(k-4) && ends("\04" "ment")){
		  k -=4;
	}else if(R2(k-3) && ends("\03" "ent")){
		  k -=3;
	}else if(R2(k-2) && ends("\02" "ou")){
		  k -=2;
	}else if(R2(k-3) && ends("\03" "ism")){
		  k -=3;
	}else if(R2(k-3) && ends("\03" "ate")){
		  k -=3;
	}else if(R2(k-3) && ends("\03" "iti")){
		  k -=3;
	}else if(R2(k-3) && ends("\03" "ous")){
		  k -=3;
	}else if(R2(k-3) && ends("\03" "ive")){
		  k -=3;
	}else if(R2(k-3) && ends("\03" "ize")){
		  k -=3;
	}else if(R2(k-3) && ends("\03" "ion") && (b[k-3]=='s' || b[k-3]=='t')){
		  k -=3;
	}
}

/*  define Step_5a as (
        ['e']
        R2 or (R1 not shortv)
        delete
    )*/
static void Step_5a(){
	if(ends("\01" "e")){

		if(R2(k-1) ||(R1(k-1) && !isShortVowelAt(p1))){
			k--;
		}

	}
}

/*define Step_5b as (
        ['l']
        R2 'l'
        delete
    )*/
static void Step_5b(){
	if(ends("\01" "l")){
			if(R2(k-1)){
				k--;
			}
		}
}

/*@description: computes the new end point of the string
 *@param: char *s pointer to string to be stemmed
 *@param: int i start index of the string
 *@param: int j last index of the string
 *@return:int k  new end index of the string
 * */
static int stem(char *s, int i, int j){

	//initializing
	b =s;
	k0 = i;
	k = j;

	Y_found = FALSE;
    p1 = limit;
    p2 = limit;

/*do(
        gopast v  gopast non-v  setmark p1
        gopast v  gopast non-v  setmark p2
    )
 * */
   setMarkers();

    Step_1a();
    Step_1b();
    Step_1c();
    Step_2();
    Step_3();
    Step_4();
    Step_5a();
    Step_5b();


    return k;
}
