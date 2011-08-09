'''
Incoming net events
'''

class NetEvent:

    messageHandler = None
    adminMessageHandler = None
    miscMessageHandler = None

    @classmethod
    def init_0(cls):
        cls.messageHandler = MessageHandler()
        cls.adminMessageHandler = AdminMessageHandler()
        #cls.miscMessageHandler = MiscMessageHandler()
    @classmethod
    def init_1(cls):
        cls.messageHandler.init()
        cls.adminMessageHandler.init()
        #cls.miscMessageHandler.init()
    @classmethod
    def register_json_events(cls, events):
        for string, function in events.items():
            cls.messageHandler.json_events[string] = function

from net_server import NetServer
from net_out import NetOut
from game_state import GameStateGlobal
from chat_server import ChatServer


class AgentMessageHandler:
    pass

class PlayerMessageHandler:
    pass

def _ping(connection, timestamp, **msg):
    connection.sendMessage.ping(timestamp)

# routes messages by msg.cmd
class MessageHandler:

    def init(self):
        pass
    def __init__(self):
        self.json_events = {} #map strings to functions

    def process_json(self, msg, connection):
        cmd = msg.get('cmd', None)
        #print "MessageHandler.process_json: " + str(msg)

        #use json_events when possible
        if self.json_events.has_key(cmd):
            self.json_events[cmd](**msg)
        elif cmd == 'ping':
            _ping(connection, **msg)
        # game state
        elif cmd == 'create_agent':
            GameStateGlobal.agentList.create(**msg)
        elif cmd == 'agent_control_state':
            self.agent_control_state(connection.id, **msg)
        elif cmd == 'agent_button_state':
            self.agent_button_state(connection.id, **msg)
        elif cmd == 'agent_position':
            self.agent_position(connection.id, **msg)
        elif cmd == 'agent_angle':
            self.agent_angle(connection.id, **msg)
        elif cmd == 'request_agent':
            self.request_agent(connection, **msg)
        elif cmd == 'request_player':
            self.request_player(connection, **msg)
            
        elif cmd == 'request_team':
            self.request_team(connection, **msg)
        elif cmd == 'request_item':
            self.request_item(connection, **msg)
        elif cmd == 'request_weapon':
            self.request_weapon(connection, **msg)
        elif cmd == 'request_projectile':
            self.request_projectile(connection, **msg)

        elif cmd == 'fire_projectile':
            self.fire_projectile(connection.id, **msg)
        elif cmd == 'reload_weapon':
            self.reload_weapon(connection.id, **msg)
        elif cmd == 'change_weapon':
            self.change_weapon(connection.id, **msg)
        elif cmd == 'drop_weapon':
            self.drop_weapon(connection.id, **msg)

        elif cmd == 'pickup_item':
            self.pickup_item(connection.id, **msg)
        elif cmd == 'drop_item':
            self.drop_item(connection.id, **msg)

        elif cmd == 'set_block':
            self.set_block(connection.id, **msg)
        elif cmd == 'hit_block':
            self.hit_block(connection.id, **msg)

        elif cmd == 'hitscan':
            self.hitscan(connection.id, **msg)

        elif cmd == 'join_team':
            self.join_team(connection.id, **msg)

        #chat
        elif cmd == 'chat':
            ChatServer.chat.received(msg, connection)
        elif cmd == 'subscribe':
            ChatServer.chat.client_subscribe(msg, connection)
        elif cmd == 'unsubscribe':
            ChatServer.chat.client_unsubscribe(msg, connection)

        # setup
        elif cmd == 'identify': #Setup client connection
            name = msg.get('name', None)
            if name is None:
                print 'msg identify - name is missing'
                return
            connection.identify(name)
        elif cmd == 'request_client_id':
            connection.send_client_id()
        #misc
        #elif cmd == 'ping'
        #    self.ping(msg, connection)
        # map
        elif cmd == 'request_chunk_list':
            self.send_chunk_list(msg, connection)
        elif cmd == 'request_chunk':
            self.request_chunk(msg, connection)
        else:
            print "MessageHandler.process_json: cmd unknown = %s" % (str(msg),)

    def pickup_item(self, client_id, **msg):
        try:
            player = GameStateGlobal.playerList.client(client_id)
        except KeyError:
            print 'msg pickup_item :: Could not find player for client'
            return
        try:
            agent_id = int(msg.get('aid', None))
            agent = GameStateGlobal.agentList[agent_id]
            if agent.team.is_viewers():
                return
        except TypeError:
            print 'msg pickup_item :: aid missing'
            return
        except ValueError:
            print 'msg pickup_item :: aid invalid'
            return
        except KeyError:
            print 'msg pickup_item :: agent %s unknown' % (agent_id,)
            return

        try:
            item_id = int(msg.get('iid', None))
            item = GameStateGlobal.itemList[item_id]
        except TypeError:
            print 'msg pickup_item :: iid missing'
            return
        except ValueError:
            print 'msg pickup_item :: iid invalid'
            return
        except KeyError:
            print 'msg pickup_item :: object %s unknown' % (item_id,)
            return

        if not player.owns(agent):
            print 'msg pickup_item :: player %s does not own agent %s' % (player.id, agent.id,)
            return
            
        slot = None
        try:
            slot = int(msg['slot'])
        except ValueError:
            print 'msg drop_item :: slot invalid'
            return
        except KeyError:
            pass

        ''' Do this check after server physics is synced n all '''
        if agent.near_item(item):
            agent.pickup_item(item, slot)
        #agent.pickup_item(item, slot)

    def drop_item(self, client_id, **msg):
        try:
            player = GameStateGlobal.playerList.client(client_id)
        except KeyError:
            print 'msg drop_item :: Could not find player for client'
            return
        try:
            agent_id = int(msg.get('aid', None))
            agent = GameStateGlobal.agentList[agent_id]
            if agent.team.is_viewers():
                print 'ignoring agent, its a viewer'
                print msg['cmd']
                return
        except TypeError:
            print 'msg drop_item :: aid missing'
            return
        except ValueError:
            print 'msg drop_item :: aid invalid'
            return
        except KeyError:
            print 'msg drop_item :: agent %s unknown' % (agent_id,)
            return

        try:
            item_id = int(msg.get('iid', None))
            item = GameStateGlobal.itemList[item_id]
        except TypeError:
            print 'msg drop_item :: iid missing'
            return
        except ValueError:
            print 'msg drop_item :: iid invalid'
            return
        except KeyError:
            print 'msg drop_item :: object %s unknown' % (item_id,)
            return

        agent.drop_item(item)

    def join_team(self, client_id, **msg):
        try:
            player = GameStateGlobal.playerList.client(client_id)
        except KeyError:
            print 'msg join_team :: Could not find player for client'
            return
        try:
            team_id = int(msg.get('team', None))
            team = GameStateGlobal.teamList[team_id]
        except TypeError:
            print 'msg join_team :: team key missing'
            return
        except ValueError:
            print 'msg join_team :: team invalid'
            return
        except KeyError:
            print 'msg join_team :: team does not exist'
            return

        GameStateGlobal.game.player_join_team(player, team)
        print GameStateGlobal.teamList
        
    def set_block(self, client_id, **msg):
        try:
            player = GameStateGlobal.playerList.client(client_id)
        except KeyError:
            print 'msg set_block :: Could not find player for client'
            return
        try:
            agent_id = int(msg.get('aid', None))
            agent = GameStateGlobal.agentList[agent_id]
            if agent.team.is_viewers():
                print 'ignoring agent, its a viewer'
                print msg['cmd']
                return
            if agent.owner != player.id:
                print 'msg set_block :: client does not own this agent'
                return
        except TypeError:
            print 'msg set_block :: aid missing'
            return
        except ValueError:
            print 'msg set_block :: aid invalid'
            return
        except KeyError:
            print 'msg set_block :: agent unknown'
            return

        try:
            block_type = int(msg.get('type', None))
        except TypeError:
            print 'msg set_block :: type missing'
            return
        except ValueError:
            print 'msg set_block :: type invalid'
            return

        try:
            block_position = msg['pos']
            block_position = list(block_position)
            assert len(block_position) == 3
        except KeyError:
            print 'msg set_block :: pos missing'
            return
        except ValueError:
            print 'msg set_block :: pos not iterable'
            return
        except AssertionError:
            print 'msg set_block :: block pos length is not 3'
            return
        x, y, z = block_position
        block = (x, y, z, block_type,)
        GameStateGlobal.terrainMap.set(*block)
        NetOut.event.set_map([block])

    def change_weapon(self, client_id, **msg):
        try:
            player = GameStateGlobal.playerList.client(client_id)
        except KeyError:
            print 'msg change_weapon :: Could not find player for client'
            return
        try:
            agent_id = int(msg.get('aid', None))
            agent = GameStateGlobal.agentList[agent_id]
            if agent.owner != player.id:
                print 'msg change_weapon :: client does not own this agent'
                return
            if agent.team.is_viewers():
                print 'ignoring agent, its a viewer'
                print msg['cmd']
                return
        except TypeError:
            print 'msg change_weapon :: aid missing'
            return
        except ValueError:
            print 'msg change_weapon :: aid invalid'
            return
        except KeyError:
            print 'msg change_weapon :: agent unknown'
            return

        try:
            active_weapon = msg['windex']
        except KeyError:
            print 'msg change_weapon :: windex (active_weapon) missing'
            return

        if active_weapon == -1: # json doesnt have None, but None is a valid input; careful, -1 is a valid index.
            active_weapon = None

        agent.set_active_weapon(active_weapon)

    def drop_weapon(self, client_id, **msg):
        try:
            player = GameStateGlobal.playerList.client(client_id)
        except KeyError:
            print 'msg drop_weapon :: Could not find player for client'
            return
        try:
            agent_id = int(msg.get('aid', None))
            agent = GameStateGlobal.agentList[agent_id]
            if agent.team.is_viewers():
                print 'ignoring agent, its a viewer'
                print msg['cmd']
                return
            if agent.owner != player.id:
                print 'msg drop_weapon :: client does not own this agent'
                return
        except TypeError:
            print 'msg drop_weapon :: aid missing'
            return
        except ValueError:
            print 'msg drop_weapon :: aid invalid'
            return
        except KeyError:
            print 'msg drop_weapon :: agent unknown'
            return

        try:
            weapon_id = msg['wid']
        except KeyError:
            print 'msg drop_weapon :: wid missing'
            return

        agent.drop_weapon(weapon_id, by_id=True)

    def hitscan(self, client_id, **msg):
        try:
            player = GameStateGlobal.playerList.client(client_id)
        except KeyError:
            print 'msg hitscan :: Could not find player for client'
            return
        firing_agent = player.agent
        if firing_agent.team.is_viewers():
                print 'ignoring agent, its a viewer'
                print msg['cmd']
                return
        weapon = firing_agent.active_weapon()
        if not weapon.hitscan:
            print 'msg hitscan :: Client sent hitscan message for non-hitscan weapon'
            return

        try:
            target = msg['target']
        except KeyError:
            print 'msg hitscan :: target missing'
            return

        try:
            type = target['type']
        except KeyError:
            print 'msg hitscan :: target type missing'
            return

        try:
            loc = target['loc']
            if type == 'block' or type == 'empty':
                assert len(loc) == 3
            elif type == 'agent':
                assert len(loc) == 2
        except KeyError:
            print 'msg hitscan :: target location missing'
            return
        except TypeError:
            print 'msg hitscan :: target location not iterable'
            return
        except AssertionError:
            print 'msg hitscan :: target location wrong length'
            return

        # add agent/projectile information to packet and forward
        NetOut.event.hitscan(target, firing_agent.id, weapon.type)
        print 'Hitscan target type %s' % (type,)
        # apply damage
        if type == 'block':
            pass
        elif type == 'agent':
            target_aid, body_part_id = loc
            try:
                target_agent = GameStateGlobal.agentList[target_aid]
            except KeyError:
                print 'msg hitscan :: target agent does not exist'
                return
            # improve damage calculation later
            print 'HITSCAN on Agent %s' % (str(target_agent.id))
            target_agent.take_damage(weapon.base_damage, firing_agent.owner)

    def hit_block(self, client_id, **msg):
        try:
            player = GameStateGlobal.playerList.client(client_id)
        except KeyError:
            print 'msg hit_block :: Could not find player for client'
            return
        try:
            agent_id = int(msg.get('aid', None))
            agent = GameStateGlobal.agentList[agent_id]
            if agent.team.is_viewers():
                print 'ignoring agent, its a viewer'
                print msg['cmd']
                return
            if agent.owner != player.id:
                print 'msg hit_block :: client does not own this agent'
                return
        except TypeError:
            print 'msg hit_block :: aid missing'
            return
        except ValueError:
            print 'msg hit_block :: aid invalid'
            return
        except KeyError:
            print 'msg hit_block :: agent unknown'
            return

        try:
            block_position = msg['pos']
            block_position = list(block_position)
            assert len(block_position) == 3
        except KeyError:
            print 'msg hit_block :: pos missing'
            return
        except ValueError:
            print 'msg hit_block :: pos not iterable'
            return
        except AssertionError:
            print 'msg hit_block :: block pos length is not 3'
            return
        x, y, z = block_position
        block = (x, y, z, 0,)
        print 'hit_block', block
        GameStateGlobal.terrainMap.set(*block)
        NetOut.event.set_map([block])

    def reload_weapon(self, client_id, **msg):
        try:
            player = GameStateGlobal.playerList.client(client_id)
        except KeyError:
            print 'msg reload_weapon :: Could not find player for client'
            return
        try:
            agent_id = int(msg.get('aid', None))
            agent = GameStateGlobal.agentList[agent_id]
            if agent.team.is_viewers():
                print 'ignoring agent, its a viewer'
                print msg['cmd']
                return
            if agent.owner != player.id:
                print 'msg reload_weapon :: client does not own this agent'
                return
        except TypeError:
            print 'msg reload_weapon :: aid missing'
            return
        except ValueError:
            print 'msg reload_weapon :: aid invalid'
            return
        except KeyError:
            print 'msg reload_weapon :: agent unknown'
            return

        try:
            weapon_type = int(msg.get('weapon', None))
        except TypeError:
            print 'msg reload_weapon :: weapon missing'
            return
        except ValueError:
            print 'msg reload_weapon :: weapon invalid'
            return
        try:
            weapon_index = [weapon.type for weapon in agent.weapons].index(weapon_type)
        except ValueError:
            print 'msg reload_weapon :: weapon unknown to agent'
            return
        weapon = agent.weapons[weapon_index]
        if weapon.reload():
            NetOut.event.agent_update(agent, 'weapons')

# deprecated
    def agent_control_state(self, client_id, **msg):
        print 'WARNING received deprecated agent_control_state msg. ignoring'
        return
        try:
            agent = GameStateGlobal.playerList.client(client_id).agent
            if agent.team.is_viewers():
                print 'ignoring agent, its a viewer'
                print msg['cmd']
                return
        except KeyError:
            print 'msg.cmd == agent_control_state, msg.id is not a known client'
            return
        except AttributeError:
            print 'msg.cmd == agent_control_state, player has no agent'
            return

        tick = msg.get('tick', None)
        if tick is None:
            print 'msg agent_control_state missing "tick"'
            return

        try:
            state = msg['state']
            state = list(state)
            assert len(state) == 6
        except KeyError:
            print 'msg agent_control_state missing "state"'
            return
        except TypeError:
            print 'msg agent_control_state :: state is not iterable'
            return
        except AssertionError:
            print 'msg agent_control_state :: state has wrong number of elements'
            return

        try:
            angle = msg['angle']
            angle = list(angle)
            assert len(angle) == 2
        except KeyError:
            print 'msg agent_control_state :: missing "angle"'
            return
        except TypeError:
            print 'msg agent_control_state :: angle is not iterable'
            return
        except AssertionError:
            print 'msg agent_control_state :: angle has wrong number of elements'
            return

        agent.set_control_state(state, angle, tick)

    def agent_angle(self, client_id, **msg):
        try:
            agent = GameStateGlobal.playerList.client(client_id).agent
            if agent.team.is_viewers():
                print 'ignoring agent, its a viewer'
                print msg['cmd']
                return
        except KeyError:
            print 'msg.cmd == agent_angle, msg.id is not a known client'
            return
        except AttributeError:
            print 'msg.cmd == agent_angle, player has no agent'
            return

        tick = msg.get('tick', None)
        if tick is None:
            print 'msg agent_angle missing "tick"'
            return

        try:
            angle = msg['angle']
            angle = list(angle)
            assert len(angle) == 2
        except KeyError:
            print 'msg agent_angle :: missing "angle"'
            return
        except TypeError:
            print 'msg agent_angle :: angle is not iterable'
            return
        except AssertionError:
            print 'msg agent_angle :: angle has wrong number of elements'
            return

        agent.set_angle(angle)

    def agent_button_state(self, client_id, **msg):
        try:
            agent = GameStateGlobal.playerList.client(client_id).agent
            if agent.team.is_viewers():
                print 'ignoring agent, its a viewer'
                print msg['cmd']
                return
        except KeyError:
            print 'msg.cmd == agent_button_state, msg.id is not a known client'
            return
        except AttributeError:
            print 'msg.cmd == agent_button_state, player has no agent'
            return

        try:
            agent_id = msg['id']
        except KeyError:
            print 'msg agent_button_state :: aid is missing'
            return

        if agent_id != agent.id:
            print 'msg agent_button_state :: aid %s does not match player\'s agent id %s' % (agent_id, agent.id,)
            return

        try:
            buttons = msg['buttons']
        except KeyError:
            print 'msg agent_button_state :: buttons missing'
            return

        btn_len = len(buttons)
        btn_size = 6
        if btn_len != btn_size:
            print 'msg agent_button_state :: buttons of wrong size (is %s should be %s)' % (btn_len, btn_size,)
            return

        old_buttons = agent.button_state
        agent.button_state = buttons

        #forward msg
        if old_buttons != buttons:
            NetOut.event.agent_button_state(agent)
            ctrl_state = agent.compute_state()
            agent.set_control_state(ctrl_state)

    def fire_projectile(self, client_id, **msg):
        try:
            player = GameStateGlobal.playerList.client(client_id)
        except KeyError:
            print 'msg fire_projectile :: Could not find player for client'
            return
        try:
            agent_id = int(msg.get('aid', None))
            agent = GameStateGlobal.agentList[agent_id]
            if agent.team.is_viewers():
                print 'ignoring agent, its a viewer'
                print msg['cmd']
                return
            if agent.owner != player.id:
                print 'msg fire_projectile :: player does not own this agent'
                return
        except TypeError:
            print 'msg fire_projectile :: agent_id is missing or not an int'
            return
        except KeyError:
            print 'msg fire_projectile :: agent %i unknown' % (agent_id,)
            return
        agent.fire_projectile()

    def send_chunk_list(self, msg, connection):
        connection.sendMessage.send_chunk_list()

    def request_chunk(self, msg, connection):
        try:
            x,y,z = msg['value']
        except KeyError:
            print 'msg request_chunk, "value" missing'
            return
        except ValueError:
            print 'msg request_chunk, "value" must be a 3 tuple'
            return
        connection.sendMessage.send_chunk(x,y,z)

    def request_agent(self, connection, **msg):
        if 'aid' not in msg:
            return
        agent_id = msg['aid']
        connection.sendMessage.send_agent(agent_id)

    def request_player(self, connection, **msg):
        if 'pid' not in msg:
            return
        connection.sendMessage.send_player(msg['pid'])

    def request_team(self, conn, **msg):
        if 'id' not in msg:
            return
        conn.sendMessage.send_team(msg['id'])
        
    def request_projectile(self, conn, **msg):
        if 'id' not in msg:
            return
        conn.sendMessage.send_projectile(msg['id'])
   
    def request_weapon(self, conn, **msg):
        if 'id' not in msg:
            return
        conn.sendMessage.send_weapon(msg['id'])
        
    def request_item(self, conn, **msg):
        if 'id' not in msg:
            return
        conn.sendMessage.send_item(msg['id'])
        
    def agent_position(self, connection_id, **msg):
        err_msg = None
        try:
            player = GameStateGlobal.playerList.client(connection_id)
        except KeyError:
            err_msg = 'could not find player for client %s' % (connection_id,)
        try:
            agent_id = msg['id']
        except KeyError:
            err_msg = 'agent id missing'
        try:
            pos = msg['pos']
        except KeyError:
            err_msg = 'agent pos missing'
        if err_msg is not None:
            print 'msg agent_position :: ' + err_msg
            print msg
            return

        try:
            agent = GameStateGlobal.agentList[agent_id]
            if agent.team.is_viewers():
                print 'ignoring agent, its a viewer'
                print msg['cmd']
                return
        except KeyError:
            err_msg = 'agent %s unknown' % (agent_id,)

        if not player.owns(agent):
            err_msg = 'player %s does not own agent %s' % (player.id, agent.id,)

        if err_msg is not None:
            print 'msg agent_position :: ' + err_msg
            print msg
            return

        #agent.pos(pos)
        agent.state = pos
        NetOut.event.agent_position(agent)
        
# handler for admin msgs
class AdminMessageHandler:

    def init(self):
        pass
    def __init__(self):
        self.register_events()
    def register_events(self):
        events = {
            'set_map' : self._set_map,
        }
        NetEvent.register_json_events(events)

    def _set_map(self, list, **msg):
        terrainMap = GameStateGlobal.terrainMap
        for x,y,z,value in list:
            terrainMap.set(x,y,z,value)
        NetOut.event.set_map(list)

#class MiscMessageHandler:
    #def init(self):
        #pass
    #def __init__(self):
        #self.register_events()
    #def register_events(self):
        #events = {
            #'ping' : self._ping,
        #}
        #NetEvent.register_json_events(events)

    #def _ping(self, timestamp, **msg):
        #NetOut.event.ping(timestamp)
