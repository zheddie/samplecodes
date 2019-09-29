class unicodeT                                                    
{                                                                    
    public static void main ( String[] args ) throws Exception       
    {                                                                
        byte[] byteArray = new byte[4];                             
        byteArray[0]= 0x0E ; // Shift Out                           
        byteArray[1] = 0x42 ;                                        
        byteArray[2] = 0x60 ;                                        
        byteArray[3] = 0x0F ; // Shift In                            
        String unicode = new String ( byteArray, "Cp939" ) ;         
        if ( unicode.length() != 1 )                                 
        {                                                            
            throw new IllegalArgumentException ( "Error" ) ;         
        }                                                            
        char c = unicode.charAt ( 0 ) ;                              
        if ( c == '\uFF0D' )                                         
        {                                                            
            System.out.println ( "FullWidth Hypen:"+c+":" ) ;               
            System.out.println ( "0x" + Integer.toHexString ( c ) ) ;
            return ;                                                 
        }                                                            
        if ( c == '\u2212' )                                         
        {                                                            
            System.out.println ( "Minus Sign:"+c+":" ) ;                    
            System.out.println ( "0x" + Integer.toHexString ( c ) ) ;
            return ;                                                 
        }                                                            
    }                                                                
}    
