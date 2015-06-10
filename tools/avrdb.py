import sqlite3
import sys


class Avr_micro:
    def __init__(self,id=0,name='foo',spm=256,boot=0xFC00,flash=0xFFFF):
        self.id = id
        self.name = name
        self.spm = spm
        self.boot = boot
        self.flash = flash
        self.bootsize = 1024
        self.table = "uC"
        self.file = "avr.db"
        self.con = sqlite3.connect(self.file)
        self.cursor = self.con.cursor()
    def check_db(self):
        try:
            self.cursor.execute('''CREATE TABLE uC
                        (ID UNSIGNED MEDIUMINT PRIMARY KEY NOT NULL,
                        NAME      TEXT              NOT NULL UNIQUE,
                        SPM_BUFF  UNSIGNED SMALLINT NOT NULL,
                        BOOTSTART UNSIGNED SMALLINT NOT NULL,
                        FLASHEND  UNSIGNED SMALLINT NOT NULL)''')
        except:
            print('Table exists...')
            self.cursor.execute("SELECT ID FROM uC")
            if self.cursor.fetchone() is None:
                print('Table is empty, needs to be populated...\n')
                return True
            else:
                return False
        else:
            print('Creating table and needs to be populated...\n')
            return True
    def exist(self):
        self.cursor.execute('SELECT ID FROM '+self.table+' WHERE ID = ? OR NAME = ?',(self.id,self.name,))
        if self.cursor.fetchone() is None:
            return False
        else:
            return True
    def load(self):
        if self.exist():
            self.cursor.execute('SELECT * FROM '+self.table+' WHERE ID = ?',(self.id,))
            i = self.cursor.fetchone()
            if i is None:
                self.cursor.execute('SELECT * FROM '+self.table+' WHERE NAME = ?',(self.name,))
                i = self.cursor.fetchone()
                if i is None:
                    return False
            self.id = i[0]
            self.name = i[1]
            self.spm = i [2]
            self.boot = i[3]
            self.flash = i[4]
            return True
        else:
            return False
    def delet(self):
        if self.exist():
            self.cursor.execute('DELETE FROM '+self.table+' WHERE ID = ?',(self.id,))
            self.con.commit()
            return True
        else:
            return False
    def upd(self,s,b,f):
        if self.exist():
            self.spm = s
            self.boot = b
            self.flash = f
            self.cursor.execute('UPDATE '+self.table+' SET SPM_BUFF = ?, BOOTSTART = ?, FLASHEND = ? WHERE ID = ?',(self.spm,self.boot,self.flash,self.id,))
            self.con.commit()
            return True
        else:
            return False
    def create(self,n,s,b,f):
        if self.exist():
            return False
        else:
            self.name = n
            self.cursor.execute('SELECT NAME FROM '+self.table+' WHERE NAME = ?',(self.name,))
            if self.cursor.fetchone() is None:
                self.spm = s
                self.boot = b
                self.flash = f
                self.cursor.execute('INSERT INTO '+self.table+' VALUES(?, ?, ?, ?, ?)',(self.id,self.name,self.spm,self.boot,self.flash,))
                self.con.commit()
                return True
            else:
                return False

if __name__ == "__main__":
    a = Avr_micro()
    mode = a.check_db()
    while True:
        if mode == 1:
            # inserting mode
            print("Enter values to create item")
            while True:
                try:
                    sig = int(input("Enter signature (ID in HEX): "),16)
                except ValueError:
                    print("Value incorrect")
                else:
                    break
            name = input("Enter name: ").lower()
            while True:
                while True:
                    try:
                        spm = int(input("Enter SPM buffer size (decimal): "))
                    except ValueError:
                        print("Value incorrect")
                    else:
                        break
                while True:
                    try:
                        boot = int(input("Enter Boot start address (bytewise in HEX): "),16)
                    except ValueError:
                        print("Value incorrect")
                    else:
                        break
                while True:
                    try:
                        flash = int(input("Enter Flash end address (bytewise in HEX): "),16)
                    except ValueError:
                        print("Value incorrect")
                    else:
                        break
                if boot + a.bootsize -1 > flash:
                    print("\nBootloader memory space have to be greater than 1024 bytes")
                    print("if FLASHEND = 0x{:02x} is correct, BOOTSTART should be 0x{:02x}".format(flash,flash-a.bootsize+1))
                elif boot % spm != 0:
                    print("\nBOOTSTART should be a multiplier of SPM_BUFFSIZE")
                elif (flash + 1) % spm != 0:
                    print("\nFLASHEND should be a multiplier of SPM_BUFFSIZE")
                else:
                    break
            a.id = sig
            if a.create(name,spm,boot,flash):
                print("Item created")
            else:
                print("Item already exist")
            mode = 0
        elif mode == 2:
            # updating mode
            while True:
                choice = input("Enter Column (ID or NAME) to update: ").upper()
                if choice == "ID":
                    choice2 = int(input("{} (in HEX format) = ".format(choice)),16)
                    break
                elif choice == "NAME":
                    choice2 = input("{} = ".format(choice))
                    a.cursor.execute('SELECT ID FROM uC WHERE NAME = ?',(choice2,))
                    b = a.cursor.fetchone()
                    if b is None:
                        choice2 = 0
                    else:
                        choice2 = b[0]
                    break
                else:
                    print("Error on input")
            a.id = choice2
            if not a.exist():
                print("Item do not exist")
            else:
                a.load()
                while True:
                    while True:
                        try:
                            spm = int(input("Enter SPM buffer size (decimal): "))
                        except ValueError:
                            print("Value incorrect")
                        else:
                            break
                    while True:
                        try:
                            boot = int(input("Enter Boot start address (bytewise in HEX): "),16)
                        except ValueError:
                            print("Value incorrect")
                        else:
                            break
                    while True:
                        try:
                            flash = int(input("Enter Flash end address (bytewise in HEX): "),16)
                        except ValueError:
                            print("Value incorrect")
                        else:
                            break
                    if boot + a.bootsize -1 > flash:
                        print("\nBootloader memory space have to be greater than 1024 bytes")
                        print("if FLASHEND = 0x{:02x} is correct, BOOTSTART should be 0x{:02x}".format(flash,flash-a.bootsize+1))
                    elif boot % spm != 0:
                        print("\nBOOTSTART should be a multiplier of SPM_BUFFSIZE")
                    elif (flash + 1) % spm != 0:
                        print("\nFLASHEND should be a multiplier of SPM_BUFFSIZE")
                    else:
                        break
                if a.upd(spm,boot,flash):
                    print("Item modified")
            mode = 0
        elif mode == 3:
            # deleting mode
            while True:
                choice = input("Enter Column (ID or NAME) to Delete: ").upper()
                if choice == "ID":
                    choice2 = int(input("{} (in HEX format) = ".format(choice)),16)
                    break
                elif choice == "NAME":
                    choice2 = input("{} = ".format(choice))
                    a.cursor.execute('SELECT ID FROM uC WHERE NAME = ?',(choice2,))
                    b = a.cursor.fetchone()
                    if b is None:
                        choice2 = 0
                    else:
                        choice2 = b[0]
                    break
                else:
                    print("Error on input")
            a.id = choice2
            if a.delet():
                print("Item deleted")
            else:
                print("Item do not exist")
            mode = 0
        elif mode == 4:
            # show table
            print("\nSignature\tName\t\tSPM_SIZE\tBOOTSTART\tFLASHEND")
            a.cursor.execute("SELECT * FROM uC")
            for i in a.cursor:
                print("0x{:02x}\t{}\t{}\t\t0x{:02x}\t\t0x{:02x}".format(i[0],i[1],i[2],i[3],i[4]))
            print("\n")
            mode = 0
        elif mode == 5:
            # exits
            print('Good bye...\n')
            break
        else:
            while True:
                try:
                    choice = int(input("\nEnter an option\n 1 - To insert\n 2 - To update\n"
                                       " 3 - To delete\n 4 - To show table\n 5 - To exit\n:"))
                except ValueError:
                    print("Input is not an integer")
                else:
                    if choice > 0 and choice < 6:
                        mode = choice
                        break
                    else:
                        print("Incorrect option")
    a.con.close()
    sys.exit()
