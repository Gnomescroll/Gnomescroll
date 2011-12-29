'''
Bernie's L System demo in Python.

This program implements a simple context free L System,
and renders two dimensional images in a window.
It needs (at least) python 2.6, and uses the turtle 
graphics library, which is based on TK graphics.

To try it out, either load into Python and run the 
demo functions, or run it from the command line:
  
    python lsystem.py

Author: Bernie Pope: www.cs.mu.oz.au/~bjpop/
Licence: unrestricted.

Feel free to play with the code as much as you like
and share your changes with the world.

Some remarks about the implementation:

An L-System is a term rewriting system made up of one or
more rules. Rules have the form:

    head -> body

where head is a variable, and body is a non-empty string
made up of variables and constants. Variables are 
denoted by upper-case alphabetic letters. Constants
are denoted by any character which is not a variable.

Here is an example L-System: 

    X -> F-[[X]+X]+F[+FX]-X
    F -> FF

In this program the convention is that the first rule
is taken as the starting point.

An LSystem object is constructed like so:

    rules = ['X -> F-[[X]+X]+F[+FX]-X', 'F -> FF'] 
    my_system = LSystem(rules)

That is, the LSystem constructor takes a list of strings
as its argument, where each string is a single rule.

An LSystem object doesn't do anything on its own - it must
be interpreted.

LSystem objects have a run method which takes two parameters:
    1) An non-negative integer which indicates how many times
       the rules should be iterated. 
    2) An interpreter object which implements an 'interpretTokens' 
       method.

Here is a simple example:

  class SimpleInterp(object):
     def interpretTokens(self, tokens): return tokens

  answer = my_system().run(6, SimpleInterp())

A more sophisticated interpreter is defined called Visualise
which renders the result of iterating an LSystem as turtle
graphics.

The Visualise constructor takes a dictionary mapping LSystem
variables to functions, here is an example:

    { '-': lambda _ : left(left_angle)
    , '+': lambda _ : right(right_angle)
    , 'F': lambda _ : forward(fwd_distance)
    , '[': lambda obj : obj.push()
    , ']': lambda obj : obj.pop()
    }

'''

import sys
# import the turtle graphics library
try:
    from turtle import *
except ImportError:
    print("This program requires the turtle graphics library.")
    print("Unfortunately Python cannot find that library on your computer.")
    print("See the documentation at: http://docs.python.org/library/turtle.html")
    sys.exit(-1)
from collections import deque

py_version = sys.version_info[:2]
if py_version < (2,6):
    print("This program requires Python version 2.6 or greater to run.")
    print("Your version of Python is " + '.'.join(map(str,py_version)) + ", which is too old.")
    sys.exit(-1)

# Some demo functions, which make it relatively easy to use

def interactive_demo():
    def show_demo(name, action):
       print(name)
       action()
       input = raw_input("Press any key to continue or q/Q to quit: ")
       if input.lower() == 'q':
          sys.exit(0)
    dragon_demo()
    #demo1()
    #show_demo("Bushy tree", demo1)
    #show_demo("Twiggy tree", demo2)
    #show_demo("Koch curve", demo3)
    #show_demo("Sierpinski triangle", demo4)
    #show_demo("Peano Gosper curve", demo5)
    #show_demo("Conifer-like tree", demo6)
    #show_demo("Tiles", demo7)
    #show_demo("Crystal", demo8)
    #show_demo("Peano curve", demo9)

def demo0():
    class SimpleInterp(object):
       def interpretTokens(self, tokens): return tokens
    return bushy_tree().run(6, SimpleInterp())

def demo1():
    def init():
       initPosition()
       left(90)
    vis = Visualise(basic_actions(25,25,5), init)
    bushy_tree().run(5,vis)

def bushy_tree():
    return LSystem(['F -> FF-[-F+F+F]+[+F-F-F]'])

def demo2():
    def init():
       initPosition()
       left(90)
    vis = Visualise(basic_actions(25,25,2), init)
    twiggy_tree().run(7,vis)

def twiggy_tree():
    rules = ['X -> F-[[X]+X]+F[+FX]-X', 'F -> FF'] 
    return LSystem(rules)

def dragon():
    rules = ['X -> X+YF', 'Y -> FX-Y', 'F -> F']
    return LSystem(rules)

def dragon_demo():
    #class SimpleInterp(object):
       #def interpretTokens(self, tokens): return tokens
    #def init(): initPosition(lambda width, height: (width/2, -height/2))
    actions = basic_actions(90,90, 1)
    #actions['-'] = lambda _ : left(90)
    #actions['+'] = lambda _ : right(90)
    def init():
       initPosition()
       left(90)
    vis = Visualise(actions, init)
    dragon().run(90, vis)

def demo3():
    def init(): initPosition(lambda width, height : (-9*width/20, -height/4))
    actions = basic_actions(None,None,0.06)
    actions['-'] = lambda _ : right(68)
    actions['+'] = lambda _ : left(68)
    vis = Visualise(actions, init)
    koch().run(7,vis)

def koch():
    return LSystem(['F -> F+F-F-F+F'])

def demo4():
    def init():
       initPosition(lambda width, height : (-3*width/8, -height/4))
    actions = basic_actions(60,60,0)
    actions['A'] = lambda _ : forward(0.5)
    actions['B'] = lambda _ : forward(0.5)
    actions['-'] = lambda _ : right(60)
    actions['+'] = lambda _ : left(60)
    vis = Visualise(actions, init)
    sierpinski().run(10,vis)

def sierpinski():
    return LSystem(['A -> B-A-B', 'B -> A+B+A'])

def demo5():
    def init():
       initPosition(lambda width, height : (width/4, 3*height/8))
    actions = basic_actions(60,60,4)
    vis = Visualise(actions, init)
    peano_gosper().run(5,vis)

def peano_gosper():
    rules = [ 'X -> X+YF++YF-FX--FXFX-YF+'
            , 'Y -> -FX+YFYF++YF+FX--FX-Y'
            , 'F -> F' ]
    return LSystem(rules)

def demo6():
    def init():
       initPosition(lambda width, height : (0, -3*height/8))
       left(90)
    actions = basic_actions(20,20,11)
    vis = Visualise(actions, init)
    conifer().run(12,vis)

def conifer():
    rules = [ 'I -> VZFFF'
            , 'V -> [+++W][---W]YV'
            , 'W -> +X[-W]Z'
            , 'X -> -W[+X]Z'
            , 'Y -> YZ'
            , 'Z -> [-FFF][+FFF]F'
            , 'F -> F' ]
    return LSystem(rules)

def demo7():
    def init():
       initPosition(lambda width, height : (-width/5, 0))
    actions = basic_actions(90,90,4)
    vis = Visualise(actions, init)
    tiles().run(6,vis)

def tiles():
    rules = [ 'I -> F+F+F+F'
            , 'F -> FF+F-F+F+FF' ]
    return LSystem(rules)

def demo8():
    def init():
       initPosition(lambda width, height : (-width/3, -height/3))
       left(90)
    actions = basic_actions(90,90,2)
    vis = Visualise(actions, init)
    crystal().run(6,vis)

def crystal():
    rules = ['I -> F+F+F+F', 'F -> FF+F++F+F']
    return LSystem(rules)

def demo9():
    def init():
       initPosition(lambda width, height : (-width/3, -height/3))
       left(90)
    actions = basic_actions(90,90,2)
    vis = Visualise(actions, init)
    peano_curve().run(5,vis)

def peano_curve():
    rules = [ 'X -> XFYFX+F+YFXFY-F-XFYFX'
            , 'Y -> YFXFY-F-XFYFX+F+YFXFY'
            , 'F -> F' ]
    return LSystem(rules)

class LSystem(object):
    def __init__ (self, rules):
       if len(rules) > 0:
          for r in rules:
             exec(compile(r)) in locals()
          firstRuleName,_ = decomposeRule(rules[0])
          exec('def start(n): return ' + firstRuleName + '(n)') in locals()
          self.rule = start
       else:
          self.rule = lambda _ : ''
    def run(self, maxIterations, interpreter):
       return interpreter.interpretTokens(self.rule(maxIterations))

class Visualise (object):
    def __init__(self, dict, initCommand=None):
       self.actions = dict
       self.initCommand = initCommand
       self.stack = deque()

    def interpretTokens(self, tokens):
       initDisplay()
       if self.initCommand != None: self.initCommand()
       def action_fun(token):
          return self.actions.get(token, lambda _ : None)(self)
       self.stack = deque() 
       map (action_fun, tokens)

    def push(self):
       orient = heading()
       pos = position()
       self.stack.append((orient, pos))

    def pop(self):
       stack = self.stack
       if len(stack) == 0:
          raise Exception('Attempt to pop empty stack')
       (orient, pos) = stack.pop()
       up()
       goto(pos) 
       setheading(orient)
       down()
       
def basic_actions (left_angle, right_angle, fwd_distance):
    return { '-': lambda _ : left(left_angle)
           , '+': lambda _ : right(right_angle)
           , 'F': lambda _ : forward(fwd_distance)
           , '[': lambda obj : obj.push()
           , ']': lambda obj : obj.pop()
           }

# Configuration of graphics window
def initDisplay(drawColour="black"):
    title ("Bernie's L System demo")
    setup()
    reset()
    degrees()
    color(drawColour)
    # Try to make the animation of drawing reasonably fast.
    tracer(50,0) # Only draw every 50th update, set delay to zero.
    hideturtle() # don't draw the turtle; increase drawing speed.

def initPosition(mover=lambda width, height : (0, -height/2)):
    height = window_height()
    width = window_width()
    up() 
    goto (mover (width, height))
    down()

'''
The input rule:
 
    X -> X+X+F

is compiled to:

    def X(n): 
        if n > 0:
           xn = X(n-1)
           fn = F(n-1)
           return ''.join([xn,'+',xn,'+',fn])
        else:
           return 'X'
'''

def compile(rule):
    (name, body) = decomposeRule(rule)
    (vars,listIds) = varsIds(body)
    defPart = 'def ' + name + '(n):'
    varBinds = list(map(mkVarBind,vars))
    joinListPart = "''.join([" + ','.join(listIds) + '])'
    ifHead = 'if n > 0:' 
    ifBody = varBinds + ['return ' + joinListPart]
    elsePart = 'else: return ' + quote(name)
    return '\n'.join(
       [defPart] + map(indent,
          [ifHead] + map(indent,
             ifBody) + [elsePart]))

def decomposeRule(rule):
    splitRule = rule.split('->')
    if len(splitRule) != 2:
       raise Exception("badly formed L-System rule: " + quote(str(rule)))
    name = splitRule[0].strip()
    body = splitRule[1].strip()
    if len(name) != 1 or len(body) == 0:
       raise Exception("badly formed L-System rule: " + quote(str(rule)))
    return (name, body)

def mkVarBind(var):
    return var.lower() + 'n = ' + var + '(n-1)'

def quote(str): return "'" + str + "'"

def indent(str): return '   ' + str

def varsIds(str):
    vars = set()
    list = []
    for c in str:
       if c.isupper():
          vars.add(c)
          list.append(c.lower()+'n')
       else:
          list.append(quote(c))
    return (vars, list)

if __name__ == "__main__":
    interactive_demo() 
