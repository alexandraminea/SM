
Rd = 0
Wr = 1
Bus_Rd = 2
Bus_Wr = 3
Flush = 4
Bus_RdX = 5
Flush_star = 6

M = 10
E = 11
S = 12
I = 13
X = 14

op = {Rd : "Rd", 
        Wr : "Wr", 
        Bus_Rd : "Bus_Rd", 
        Bus_Wr : "Bus_Wr", 
        Flush : "Flush", 
        Bus_RdX : "Bus_RdX",
        Flush_star : "Flush*", 
        }
states = {E : "E", M : "M", S : "S", I : "I"}

class TestMesi:
    def __init__(self):
        self.test_list = [
            (0, Rd, 0),
            (1, Rd, 0),
            (2, Rd, 0),
            (0, Rd, 0),
            (0, Wr, 0),
            (0, Rd, 0),
            (1, Wr, 0),
            (2, Rd, 0)    
        ]
        self.mesi = Mesi()

    def test(self):
        i = 1
        print(f'---------- INSTR  {0} ----------')
        print(f"PROCESSORS STATES: {['I'] * 4}")
        for t in self.test_list:
            print(f'---------- INSTR  {i} ----------')
            print(f'Processor {t[0] + 1} operation: {op[t[1]]} at addr {t[2]}')
            self.mesi.instruction(t[0], t[1], t[2])
            print(f"PROCESSORS STATES: {self.mesi.bus.status()}")
            i += 1

class Mesi:   
    def __init__(self):
        self.memory = Memory()
        self.bus = Bus(self.memory)
        self.processors = [Processor(i, self.bus, self.memory) for i in range(4)]

    def instruction(self, pr_id, operation, addr):
        if operation == Rd:
            self.processors[pr_id].read(addr)
        else:
            self.processors[pr_id].write(addr)

class Processor:
    def __init__(self, id, bus, memory):
        self.id = id
        self.cache = {'state' : I, 'val' : [0, 0, 0, 0]}
        self.bus = bus
        self.bus.processors.append(self)
        self.memory = memory

    def read(self, addr):
        if self.cache['state'] == I:
            # bus read op I -> E / S
            self.cache = self.bus.transition({'id': self.id, 'op': Bus_Rd})
            state = states[self.cache['state']]
        else:
            # no bus op
            pass
        
        return self.cache

    def write(self, addr):
        if self.cache['state'] in [I, S]:
            # bus read X op I/S -> M
            self.cache = self.bus.transition({'id': self.id, 'op': Bus_RdX})

    def bus_transition(self, proc_operation):
        id = proc_operation['id']
        operation = proc_operation['op']
        if id != self.id:
            if operation == Bus_Rd:
                if self.cache['state'] in [M, E, S]:
                    self.bus.transition({'id': self.id, 'op' : Flush})
                    self.cache['state'] = S
                    self.bus.temp_values = self.cache['val']                  
            elif operation == Bus_RdX:
                if self.cache['state'] in [M, E]:
                    self.bus.transition({'id': self.id, 'op' : Flush})
                    self.cache['state'] = I
                    self.bus.temp_values = self.cache['val']
                elif self.cache['state'] == S:
                    self.bus.transition({'id': self.id, 'op' : Flush_star})
                    self.cache['state'] = I
                    self.bus.temp_values = self.cache['val']


class Memory:
    def __init__(self):
        self.memory_values = [100, 200, 300, 400]

class Bus:
    def __init__(self, memory):
        self.processors = []
        self.operations = []
        self.temp_values = None
        self.memory = memory

    def transition(self, bus_operation):
        self.operations.append(bus_operation)
        for proc in self.processors:
            proc.bus_transition(bus_operation)
        _id = bus_operation['id'] + 1
        if bus_operation['op'] == Bus_Rd:
            print(f'P{_id} BUS ACTION: {op[Bus_Rd]}')
            return self.bus_Rd()
        elif bus_operation['op'] == Bus_RdX:
            print(f'P{_id} BUS ACTION: {op[Bus_RdX]}')
            return self.bus_RdX()
        elif bus_operation['op'] == Flush:
            print(f'P{_id} BUS ACTION: {op[Flush]}')
            return self.bus_Flush()
        elif bus_operation['op'] == Flush_star:
            print(f'P{_id} BUS ACTION: {op[Flush_star]}')
            return self.bus_Flush()

    def bus_Rd(self):
        # empty => choose E (S#)
        last_id = self.operations[-1]['id'] + 1
        if not self.temp_values:
            print(f'DATA SOURCE: Mem')
            return {'state' : E, 'val' : self.memory.memory_values}
        else:
            print(f'DATA SOURCE: Cache({last_id})')
            tmp = self.temp_values
            self.temp_values = None
            return {'state' : S, 'val' : tmp}

    def bus_RdX(self):
        if not self.temp_values:
            return {'state' : M, 'val' : self.memory.memory_values}
        else:
            last_operation = self.operations[-1]['op']
            last_id = self.operations[-1]['id'] + 1
            if last_operation == Flush:
                print(f'DATA SOURCE: Flush({last_id})')
            elif last_operation == Flush_star:
                print(f'DATA SOURCE: Flush*({last_id})')
            tmp = self.temp_values
            self.temp_values = None
            return {'state' : M, 'val' : tmp}

    def bus_Flush(self):
        # flush the bus data in the main memory
        self.memory.memory_values = self.temp_values
        return {'state' : X, 'val' : self.memory.memory_values}


    def status(self):
        status = []
        for x in self.processors:
            status.append(states[x.cache['state']])
        return status

t = TestMesi()
t.test()