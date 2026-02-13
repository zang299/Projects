
// Name: Thian Zhang Hao
// CSC 2000 – Class Project
// Date: 30-11-24
// Description of program: Creates a GUI based application for atbash and rsa cryptography

import javax.swing.*;
import java.awt.event.*;

import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.logging.Level;
import java.util.logging.Logger;




public class Project1 extends JFrame{
    
    // creates the attributes for every part of the GUI, the scanner for the rsa object and variables used for the encryption and decryption of the message
    
    private JRadioButton atbashkey1;
    private JRadioButton atbashkey2;
    private JRadioButton rsakey1;
    private JRadioButton rsakey2;
    private JRadioButton rsakey3;
    private JRadioButton rsakey4;
    private JTextField input;
    private JTextField output;
    private RSA rsa;
    private int keylength;
    private String text;
    private JCheckBox atbashcipher;
    private JCheckBox rsacipher;
    
    // creates a hashmap that matches the character to another character based on the atbash cipher for key 1
    static HashMap<Character, Character> atbashkey1_table = new HashMap<Character, Character>(){{
        put('A', 'Z'); put('B', 'Y'); put('C', 'X'); put('D', 'W'); put('E', 'V'); put('F', 'U');
        put('G', 'T'); put('H', 'S'); put('I', 'R'); put('J', 'Q'); put('K', 'P'); put('L', 'O');
        put('M', 'N'); put('N', 'M'); put('O', 'L'); put('P', 'K'); put('Q', 'J'); put('R', 'I');
        put('S', 'H'); put('T', 'G'); put('U', 'F'); put('V', 'E'); put('W', 'D'); put('X', 'C');
        put('Y', 'B'); put('Z', 'A'); put('0','9'); put('1','8'); put('2','7'); put('3','6'); put('4','5'); 
        put('5','4'); put('6','3'); put('7','2'); put('8','1'); put('9','0');
    }};
    
    // creates a hashmap that matches the character to another character based on the atbash cipher for key 2
    static HashMap<Character, Character> atbashkey2_table = new HashMap<Character, Character>(){{
        put('A', 'Z'); put('B', 'Y'); put('C', 'X'); put('D', 'W'); put('E', 'V'); put('F', 'U');
        put('G', 'T'); put('H', 'S'); put('I', 'R'); put('J', 'Q'); put('K', 'P'); put('L', 'O');
        put('M', 'N'); put('N', 'A'); put('O', 'B'); put('P', 'C'); put('Q', 'D'); put('R', 'E');
        put('S', 'F'); put('T', 'G'); put('U', 'H'); put('V', 'I'); put('W', 'J'); put('X', 'K');
        put('Y', 'L'); put('Z', 'M'); put('0','9'); put('1','8'); put('2','7'); put('3','6'); put('4','5'); 
        put('5','0'); put('6','1'); put('7','2'); put('8','3'); put('9','4');
    }};
    
    // creates a hashmap that matches the character to another character for decryption the characters on key 2 
    static HashMap<Character, Character> atbashkey2decrypt_table = new HashMap<Character, Character>(){{
        put('Z', 'A'); put('Y', 'B'); put('X', 'C'); put('W', 'D'); put('V', 'E'); put('U', 'F');
        put('T', 'G'); put('S', 'H'); put('R', 'I'); put('Q', 'J'); put('F', 'K'); put('O', 'L');
        put('N', 'M'); put('A', 'N'); put('B', 'O'); put('C', 'P'); put('D', 'Q'); put('E', 'R');
        put('F', 'S'); put('G', 'T'); put('H', 'U'); put('I', 'V'); put('J', 'W'); put('K', 'X');
        put('L', 'Y'); put('M', 'Z'); put('9','0'); put('8','1'); put('7','2'); put('6','3'); put('5','4'); 
        put('0','5'); put('1','6'); put('2','7'); put('3','8'); put('4','9');
    }};
    
    
    // creates the file input output variables for encrypt.txt, public.txt and capitals.txt
    private static Formatter outputfile;
    private static Formatter outputpublic;
    private static Scanner inputfile;
    
    
    
    public static String atbash1(String message)
    {
        // creates a blank string for the cipher
        String cipher = "";
        for(char letter : message.toCharArray())
        {
            // Checking for space
            if(letter != ' ')
            {
                // Adds the corresponding letter from the lookup_table
                cipher += Character.toLowerCase(atbashkey1_table.get(Character.toUpperCase(letter)));
            }
            else
            {
                // Adds space
                cipher += ' ';
            }
        }
        // returns the cipher
        return cipher;
    }
    
    public static String atbash2(String message)
    {
        // creates a blank string for the cipher
        String cipher = "";
        for(char letter : message.toCharArray())
        {
            // Checking for space
            if(letter != ' ')
            {
                // Adds the corresponding letter from the lookup_table
                cipher += Character.toLowerCase(atbashkey2_table.get(Character.toUpperCase(letter)));
            }
            else
            {
                // Adds space
                cipher += ' ';
            }
        }
        // returns the cipher
        return cipher;
    }
    
    public static String atbash2decrypt(String message)
    {
        String cipher = "";
        for(char letter : message.toCharArray())
        {
            if(letter != ' ')
            {
                // Check if the letter exists in the table before attempting to transform
                if (atbashkey2decrypt_table.containsKey(Character.toUpperCase(letter))) {
                    cipher += Character.toLowerCase(atbashkey2decrypt_table.get(Character.toUpperCase(letter)));
                } else {
                    // If character is not in the table, leave it unchanged
                    cipher += letter;
                }
            }
            else
            {
                cipher += ' ';
            }
        }
        return cipher;
    }   
    ActionListener checkboxaction = new ActionListener()
    {
        @Override
        public void actionPerformed(ActionEvent event)
        {
            // sets it so that if the atbash checkbox is selected everything related to rsacipher will be disabled
            if(atbashcipher.isSelected())
            {
                rsacipher.setEnabled(false);
                rsakey1.setEnabled(false);
                rsakey2.setEnabled(false);
                rsakey3.setEnabled(false);
                rsakey4.setEnabled(false);
            }
            // does the same for the rsacipher checkbox
            else if(rsacipher.isSelected())
            {
                atbashcipher.setEnabled(false);
                atbashkey1.setEnabled(false);
                atbashkey2.setEnabled(false);
            }
            else
            {
                // if not everything is set to true
                atbashcipher.setEnabled(true);
                rsacipher.setEnabled(true);
                rsakey1.setEnabled(true);
                rsakey2.setEnabled(true);
                rsakey3.setEnabled(true);
                rsakey4.setEnabled(true);
                atbashcipher.setEnabled(true);
                atbashkey1.setEnabled(true);
                atbashkey2.setEnabled(true);
            }
        }
    };
    
    ActionListener publickeytofileAction = new ActionListener()
    {
        @Override
        public void actionPerformed(ActionEvent event)
        {
            if(rsakey1.isSelected())
            {
                // sets the keylength to 4096
                keylength = 4096;
                // creates a rsa object using the keylength
                rsa = new RSA(keylength);
                try {
                    // creates the Formatter variable to write to public.txt
                    outputpublic = new Formatter("public.txt");
                } catch (FileNotFoundException ex) {
                    System.out.println("filenot found");
                }
                // writes down the public key to public.txt
                outputpublic.format("e: %s\n", rsa.getE().toString());
                outputpublic.format("N: %s\n", rsa.getN().toString());
                // closes the file
                outputpublic.close();
            }
            
            else if(rsakey2.isSelected())
            {
                // does the same as above for 8192 keylength
                keylength = 8192;
                rsa = new RSA(keylength);
                try {
                    outputpublic = new Formatter("public.txt");
                } catch (FileNotFoundException ex) {
                    System.out.println("filenot found");
                }
                outputpublic.format("e: %s\n", rsa.getE().toString());
                outputpublic.format("N: %s\n", rsa.getN().toString());
                outputpublic.close();
            }
            
            else if(rsakey3.isSelected()) {
            keylength = 4096;
            rsa = new RSA(keylength);

                try {
                    // creates a thread for RSA encryption file output
                    outputpublic = new Formatter("public.txt");
                } catch (FileNotFoundException ex) {
                    Logger.getLogger(Project1.class.getName()).log(Level.SEVERE, null, ex);
                }
                // creates a new thread to execute the following
            new Thread(() -> {
                // ensures synchronized access to Formatter
                synchronized (outputpublic) {
                    outputpublic.format("e: %s\n", rsa.getE().toString());
                    outputpublic.format("N: %s\n", rsa.getN().toString());
                }
                outputpublic.close();  
            }).start();
            }

        
        
        // Start the writing task in a new thread
               
        else if(rsakey4.isSelected())
        {
            // does the same as 8192 keylength
            keylength = 8192;
            rsa = new RSA(keylength);

            try {
                
                outputpublic = new Formatter("public.txt");
            } catch (FileNotFoundException ex) {
                Logger.getLogger(Project1.class.getName()).log(Level.SEVERE, null, ex);
            }
            new Thread(() -> {
                
                synchronized (outputpublic) {
                    outputpublic.format("e: %s\n", rsa.getE().toString());
                    outputpublic.format("N: %s\n", rsa.getN().toString());
                }
                outputpublic.close();  
            }).start();
                
        }
            
        }       
        
    };
    
    
    
    ActionListener encryptAction = new ActionListener()
    {
        @Override
        public void actionPerformed(ActionEvent event)
        {
            try {
                // opens write variable to encrypt.txt
                outputfile = new Formatter("encrypt.txt");
            } catch (FileNotFoundException ex) {
                System.out.println("Error opening file...");
            }
            if(atbashkey1.isSelected())
            {
                // gets the message from input textfield
                String message = input.getText();
                // writes to encrypt.txt the encrypted message by calling atbash1 on the message which encrypts it
                outputfile.format("%s", atbash1(message));
                // closes the file
                outputfile.close();
            }
            else if(atbashkey2.isSelected())
            {
                // does the same but calls the other atbash key cipher
                String message = input.getText();
                outputfile.format("%s", atbash2(message));
                outputfile.close();
            }
            try{
                if(rsakey1.isSelected() || rsakey2.isSelected())
                {
                    // gets the message from input textfield
                    String message = input.getText();
                    // opens write file to encrypt.txt
                    FileOutputStream outputrsafile = new FileOutputStream("encrypt.txt");
                    // runs the encrypt message method from rsa for the message and stores it in a byte array
                    byte[] cipher = rsa.encryptMessage(message.getBytes());
                    // writes it to encrypt.txt
                    outputrsafile.write(cipher);
                    // closes the file
                    outputrsafile.close();
                    
                }
            } catch (FileNotFoundException ex) {
                Logger.getLogger(Project.class.getName()).log(Level.SEVERE, null, ex);
            } catch (IOException ex) {
                Logger.getLogger(Project.class.getName()).log(Level.SEVERE, null, ex);
            }
            
            if(rsakey3.isSelected() || rsakey4.isSelected())
                {
                    // creates a new thread
                    new Thread(() -> {
                        try{
                            // does the same as above
                            String message = input.getText();
                            FileOutputStream outputrsafile = new FileOutputStream("encrypt.txt");
                            byte[] cipher = rsa.encryptMessage(message.getBytes());
                            outputrsafile.write(cipher);
                            outputrsafile.close();
                        } catch (FileNotFoundException ex) {
                            Logger.getLogger(Project1.class.getName()).log(Level.SEVERE, null, ex);
                        } catch (IOException ex) {
                            Logger.getLogger(Project1.class.getName()).log(Level.SEVERE, null, ex);
                        }
                        // starts the thread
                    }).start();
                    
                    
                    
                }
                    
            
           
        }
    };
    
    ActionListener decryptAction = new ActionListener()
    {
        @Override
        public void actionPerformed(ActionEvent event)
        {
            try {
                // initializes input file to the read file encrypt.txt
                inputfile = new Scanner(Paths.get("encrypt.txt"));
            } catch (FileNotFoundException ex) {
                System.err.println("File not found: " + ex.getMessage());
            } catch (IOException ex) {
                System.err.println("IO error: " + ex.getMessage());
            }     
            
            
            while(inputfile.hasNextLine())
            {
                // sets secretmessage to the inputfile line
                String secretmessage = inputfile.nextLine();
                
                // if the first atbash key radiobutton is selected
                if(atbashkey1.isSelected())
                {
                    // sets text to call atbash1 of secretmessage since calling it again decrypts it back
                    text=atbash1(secretmessage);
                }
                else if(atbashkey2.isSelected())
                {
                    // sets text to call atbash2decrypt of secret message
                    text=atbash2decrypt(secretmessage);
                }
               
            }
            
            try{
                // if the non threaded first rsakey or second rsakey is selected
                if(rsakey1.isSelected() || rsakey2.isSelected())
                {
                    // creates fileinputstream read variable to encrypt.txt file
                    FileInputStream inputrsafile = new FileInputStream("encrypt.txt");
                    // sets cipher byte array to all the bytes in the encrypt.txt file
                    byte[] cipher = inputrsafile.readAllBytes();
                    // closes the file
                    inputrsafile.close();
                    
                    // sets plain byte array to decryptMessage method of cipher
                    byte[] plain = rsa.decryptMessage(cipher);
                    // sets text to the string converted plain
                    text = new String(plain);
                    
                    
                }
            } catch (FileNotFoundException ex) {
                Logger.getLogger(Project.class.getName()).log(Level.SEVERE, null, ex);
            } catch (IOException ex) {  
                Logger.getLogger(Project.class.getName()).log(Level.SEVERE, null, ex);
            }   
            
            
                 if(rsakey3.isSelected() || rsakey4.isSelected())
                 {
                    // creates a new thread
                    new Thread(() -> {
                        try{
                            // does the same as above
                            FileInputStream inputrsafile = new FileInputStream("encrypt.txt");
                            byte[] cipher = inputrsafile.readAllBytes();
                            inputrsafile.close();   
                            
                            byte[] plain = rsa.decryptMessage(cipher);
                            text = new String(plain);
                            
                        } catch (FileNotFoundException ex) {
                            Logger.getLogger(Project1.class.getName()).log(Level.SEVERE, null, ex);
                        } catch (IOException ex) {
                            Logger.getLogger(Project1.class.getName()).log(Level.SEVERE, null, ex);
                        }
                        // runs the thread
                    }).start();
                    
                    
                    
                }
            
            
            
            // sets the output jtextfield to the text variable and closes inputfile
            output.setText(text);
            inputfile.close();

            }
            
            
        };
    
    
    
    
    public Project1()
    {
        
        super("Project");
        // sets layout to flowlayout center
        setLayout(new FlowLayout(FlowLayout.CENTER));
        
        // creates new panel called input panel and sets it to 2x1 grid layout
        JPanel inputpanel = new JPanel();
        inputpanel.setLayout(new GridLayout(2,1));
        
        // sets input to a textfield size 20
        input = new JTextField(20);
        // sets encryptlabel to a jlabel telling the user its a text message to encrypt
        JLabel encryptlabel = new JLabel("Text Message To Encrypt");
        
        // adds the input textfield and encrypt label to the input panel
        inputpanel.add(input);
        inputpanel.add(encryptlabel);
        // creates two checkboxes for atbash and rsa cipher
        atbashcipher = new JCheckBox("Atbash Cipher");
        rsacipher = new JCheckBox("RSA Cipher");
        // adds both checkboxes to actionlistener "checkboxaction"
        atbashcipher.addActionListener(checkboxaction);
        rsacipher.addActionListener(checkboxaction);
        // intializes the atbash keys to key 1 and key 2 
        atbashkey1 = new JRadioButton("Key 1");
        atbashkey2 = new JRadioButton("Key 2");
        // creates the button group atbashkeys to store the two atbash keys radio buttons
        ButtonGroup atbashkeys = new ButtonGroup();
        atbashkeys.add(atbashkey1);
        atbashkeys.add(atbashkey2);
        // creates a panel for the atbash keys radio buttons
        JPanel atbashkeyspanel = new JPanel();
        atbashkeyspanel.setLayout(new GridLayout(2,1));
        // adds both radio buttons to the atbashkeyspanel
        atbashkeyspanel.add(atbashkey1);
        atbashkeyspanel.add(atbashkey2);
        
        // creates a new panel called atbashpanel
        JPanel atbashpanel = new JPanel();
        // sets the panels layout to flow layout
        atbashpanel.setLayout(new FlowLayout());
        // adds the atbash label and atbashkeys panel to atbashpanel
        atbashpanel.add(atbashcipher);
        atbashpanel.add(atbashkeyspanel);
        
        // creates a new panel called rsakeyspanel and buttongroup for the rsakeys radio buttons
        JPanel rsakeyspanel = new JPanel();
        ButtonGroup rsakeys = new ButtonGroup();
        rsakey1 = new JRadioButton("Key Length: 4096");
        rsakey2 = new JRadioButton("Key Length: 8192");
        rsakey3 = new JRadioButton("Key Length: 4096 + Threads");
        rsakey4 = new JRadioButton("Key Length: 8192 + Threads");
        // sets rsakeyspanel grid layout to 4x1
        rsakeyspanel.setLayout(new GridLayout(4,1));
        // adds rsacipher to rsakeyspanel
        rsakeyspanel.add(rsacipher);
        // adds the rsakeys to the rsakeys button group
        rsakeys.add(rsakey1);   
        rsakeys.add(rsakey2);
        rsakeys.add(rsakey3);
        rsakeys.add(rsakey4);
        // also adds it to the rsakeyspanel
        rsakeyspanel.add(rsakey1);
        rsakeyspanel.add(rsakey2);
        rsakeyspanel.add(rsakey3);
        rsakeyspanel.add(rsakey4);
        // creates new rsapanel and sets it to flowlayout and adds the checkbox and the rsakeyspanel
        JPanel rsapanel = new JPanel();
        rsapanel.setLayout(new FlowLayout());
        rsapanel.add(rsacipher);
        rsapanel.add(rsakeyspanel);
        
        // creates a panel for the ciphering part and adds both atbash and rsa panels
        JPanel cipherpanel = new JPanel();
        cipherpanel.setLayout(new BoxLayout(cipherpanel, BoxLayout.Y_AXIS));
        cipherpanel.add(atbashpanel);
        cipherpanel.add(rsapanel);
        
        
        // creates the buttonspanel and sets it to flowlayout
        JPanel buttonspanel = new JPanel();
        buttonspanel.setLayout(new FlowLayout());
        
        // creates the specific buttons and adds their respective action listeners
        JButton publickeytofile = new JButton("Public Key to File");
        publickeytofile.addActionListener(publickeytofileAction);
        JButton encrypt = new JButton("Encrypt to file");
        encrypt.addActionListener(encryptAction);
        JButton decrypt = new JButton("Decrypt from file");
        decrypt.addActionListener(decryptAction);
        
        buttonspanel.add(publickeytofile);
        buttonspanel.add(encrypt);
        buttonspanel.add(decrypt);
        
        // output textfield sets to size 20
        output = new JTextField(20);
        // sets it so that the user cant edit it
        output.setEditable(false);
        
        // creates a new label to tell the user that the textfield is for decrypted message
        JLabel textmessagedecrypt = new JLabel("Text Message Decrypted");
        
        // creates a new panel set to 2x1 grid layout and adds the output textfield and textmessagedecrypt label
        JPanel decryptpanel = new JPanel();
        decryptpanel.setLayout(new GridLayout(2,1));
        decryptpanel.add(output);
        // adds output and textmessagedecrypt to the decrypt panel
        decryptpanel.add(textmessagedecrypt);
        
        // encrypt label and textmessagedecrypt is set to the center
        encryptlabel.setHorizontalAlignment(SwingConstants.CENTER);
        textmessagedecrypt.setHorizontalAlignment(SwingConstants.CENTER);
        // adds all the panels to the main panel
        add(inputpanel);
       
        add(cipherpanel);
        add(buttonspanel);
        add(decryptpanel);
        
        
    }
    
     public static void main(String[] args) {
        // runs the constructor and creates the gui with the corresponding size
        Project1 cipher = new Project1();
        cipher.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        cipher.setSize(420, 350);
        cipher.setVisible(true);
    }
    
    
    }
