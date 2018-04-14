#include "Simulated_Annealing.h"

bool double_equals(double, double, double);

Node::Node(){
name = "-1";
height = 0;
width = 0;
nodeIndex = 0;
}

operandInfo::operandInfo(){
name = "0";
area = 0;
aspectRatio = 0;
operandIndex = 0;
}

operandInfo::operandInfo(string name, double area, double aspectRatio){
this->name = name;
this->area = area;
this->aspectRatio = aspectRatio;
}

void Anneal::getBest(){
    returnBest();
}
void Anneal::returnBest(){
    cout << "Best NPE: " << Best << endl;
    cout << "Cost: " << calcCost(Best) << endl;
}

void Anneal::moveM1(string & obj){
    M1(obj);
}

void Anneal::moveM2(string & obj){
    M2(obj);
}

void Anneal::moveM3(string & obj){
    M3(obj);
}

void Anneal::setE0(string NPE){
    E0 = NPE;
    this->NPE = E0;
    Best = E0;
}

void Anneal::setT0(double input){
    if(input == -1){
        T0 = input * deltaAverage() * (1/log(P));
        T = T0;
    }
    else{
        T0 = input;
        T = T0;
    }
}

double Anneal::deltaAverage(){
 int uphill = 0;
 double sumDif = 0.0;
 double cost;
 string tempNPE = NPE;
 double initalCost = calcCost(NPE); //Inital cost
        
     while(uphill < 11){
         moveSelect(tempNPE); //perform random move
         cost = calcCost(tempNPE);
         if(cost > initalCost){
            cout << "initalCost: " << initalCost << endl;
            cout << "Cost: " << cost << endl;
            cout << "cost - initalCost: " << cost-initalCost << endl;
            uphill++;
            sumDif += (cost - initalCost);
            cout << "sum of Differences: " << sumDif << endl;
            }
        }
            return sumDif/10;
}

void Anneal::moveSelect(string & tempNPE){
    int choice = rand()%3;
    switch(choice){
        case 0:
            moveM1(tempNPE);
          break;
        case 1:
            moveM2(tempNPE);
        break;
         case 2:
            moveM3(tempNPE);
           break;
        default:
          cout << "Error. Move was not selected!" << endl;
          break;
        }
}

//Select two adjacent operands ei and ej;
//VERIFIED WORKING!!!
void Anneal::M1(string & NPE){
int npeSize = NPE.size();
int eleBoundary = npeSize - 2;
int searchElement = rand()%(npeSize - 1);
int attempts = 0;
int valid1 = 0;
int valid2 = 0;

do{
    //reset values of valid to 0
    if(NPE[searchElement] == 'V'){ //Is the element a V?
        //the element was a V!
        while(searchElement <= eleBoundary){ //While searchElement selected is less than
            searchElement++;                 // or equal to eleBoundary move to the right 
            if(NPE[searchElement] != 'V' && NPE[searchElement] != 'H'){
                //If new element is not a V and not an H we have an operand!
                if(!valid1) { valid1 = searchElement;} // If !valid = 1 (i.e. it is zero)
                // set it equal to search element
                else if (valid1 && !valid2){ //Unsure how this is going to behave
                    valid2 = searchElement;
                    swap(NPE[valid1], NPE[valid2]);
                    return;
                }
            }
        }
    }
    else if(NPE[searchElement] == 'H'){
        while(searchElement > 0){ //While searchElement selected is less than
            searchElement--;      // or equal to eleBoundary move to the right 
            if(NPE[searchElement] != 'V' && NPE[searchElement] != 'H'){
                //If new element is not a V and not an H we have an operand!
                if(!valid1) { valid1 = searchElement;} // If !valid = 1 (i.e. it is zero)
                // set it equal to search element
                else if (valid1 && !valid2){ //Unsure how this is going to behave
                    valid2 = searchElement;
                    swap(NPE[valid1], NPE[valid2]);
                    return;
                }
            }
        }
      
    }
    else{
        //cout << "NOT H or V " << endl;
        valid1 = searchElement;
        while(searchElement <= eleBoundary){ //While searchElement selected is less than
            searchElement++;                              // or equal to eleBoundary move to the right 
            if(NPE[searchElement] != 'V' && NPE[searchElement] != 'H'){
                    valid2 = searchElement;   
                    swap(NPE[valid1], NPE[valid2]);
                    return;
                }
            }
        }
    
    attempts++;
    valid1 = 0;
    valid2 = 0;
    searchElement = rand()%(npeSize - 1);
}while(attempts < 5 ); // Try to search for a valid element 5 times

}

//Complement chain of operators
void Anneal::Complement(string & NPE, int start, int end){
    for(int i = start; i < end + 1; i++){
        if(NPE[i] == 'V'){
            NPE.replace(i, 1, "H");
        }
        else{
            NPE.replace(i, 1, "V");
        }
    }
}

// complement chain of operators greater than 0
// VERIFIED WORKING
void Anneal::M2(string & NPE){
int npeSize = NPE.size();
int eleBoundary = npeSize;
int searchElement = rand()%npeSize;
int attempts = 0;
int rightElement = 0;
int leftElement = 0;
int start = 0;
int end = 0;
int tempEle = searchElement;
bool foundOperatorRight = true;
bool foundOperatorLeft = true;

    do{
        switch(NPE[searchElement]){
            case 'V':
            case 'H':
            //Reinitalize variables
            foundOperatorRight = true;
            foundOperatorLeft = true;
            rightElement = 0;
            leftElement = 0;
            //Check the right
             while(foundOperatorRight){
                    tempEle++;
                    if(tempEle > npeSize -1){ // Out of bounds!
                        foundOperatorRight = false; //exit the loop
                    }
                    else if(NPE[tempEle] == 'V' || NPE[tempEle] == 'H'){ //found another link right!
                        rightElement++;
                    }
                    else{
                        foundOperatorRight = false;
                    }
                }
            //Check the left
            tempEle = searchElement;
            while(foundOperatorLeft){
                tempEle--;
                if(tempEle < 0){ // Out of bounds!
                    foundOperatorLeft = false; //exit the loop
                }
                else if(NPE[tempEle] == 'V' || NPE[tempEle] == 'H'){ //found another link left!
                    leftElement++;
                }
                else{
                    foundOperatorLeft = false;
                }
            }
            //Calculate start and end of chain
            start = searchElement - leftElement;
            end = searchElement + rightElement;
            Complement(NPE, start, end);
            attempts = 5; //exit outer while loop           
            break;
            default:
            attempts++;
            searchElement = rand()%npeSize;
            tempEle = searchElement;
            break;
           }
    }while(attempts < 5 );
}

//Check if the balloting property is satisfied
void Anneal::BallotProp(string & NPE, bool & balloting){
    int operators = 0;
    int operands = 0;
    for(int i = 0; i < NPE.size(); i++){
        if(NPE[i] == 'V' || NPE[i] == 'H'){
            operators++;
        }
        else{
            operands++;
        }
        if(operators >= operands){
            balloting = false;
            return; // failed
        }
    }
}

//check if the normalized property is satisfied
void Anneal::Nomralized(string & NPE, bool & normalized){
    for(int i = 0; i < NPE.size()-1; i++){
        if(NPE[i] == NPE[i+1]){
            normalized = false;
            return;
        }
    }
    //else we finish our loop, stays true;
}

//swap two adjacent operator and operands
// VERIFIED WORKING
void Anneal::M3(string & NPE){
    int npeSize = NPE.size();
    int eleBoundary = npeSize;
    int searchElement = rand()%npeSize;
    int attempts = 0;
    int tempEle = searchElement;
    bool foundOperatorRight = true;
    bool foundOperatorLeft = true;
    bool balloting = true;
    bool normalized = true;

    do{
        switch(NPE[searchElement]){
            case 'V':
            case 'H':
            foundOperatorRight = true;
            foundOperatorLeft = true;
            balloting = true;
            normalized = true;
            //Check the right
            tempEle++;
            if(tempEle > npeSize -1){ // Out of bounds!
                 foundOperatorRight = false; //check left
            }
            else if(NPE[tempEle] == 'V' || NPE[tempEle] == 'H'){ //another operator, need operand, check left
                foundOperatorRight = false;
             }
             else{
                swap(NPE[tempEle], NPE[searchElement]);
                BallotProp(NPE, balloting);
                Nomralized(NPE, normalized);
                if(balloting && normalized){
                    //exit loop
                    attempts = 5;
                }
                else{
                 //undo swap
                 swap(NPE[tempEle], NPE[searchElement]);
                 foundOperatorRight = false;
                }

             }
            //If right faild check the left
            if(!foundOperatorRight){
            balloting = true;
            normalized = true;
            tempEle = searchElement;
            tempEle--;
            if(tempEle < 0){ // Out of bounds!
                  foundOperatorLeft = false; //exit the loop
             }
             else if(NPE[tempEle] == 'V' || NPE[tempEle] == 'H'){ //found another link left!
                  foundOperatorLeft = false;
             }
             else{
                swap(NPE[tempEle], NPE[searchElement]);
                BallotProp(NPE, balloting);
                Nomralized(NPE, normalized);
                if(balloting && normalized){
                    //exit loop
                    attempts = 5;
                }
                else{
                 //undo swap
                 swap(NPE[tempEle], NPE[searchElement]);
                 foundOperatorLeft = false;
                }
             }
                
            }
            if(!foundOperatorRight && !foundOperatorLeft){ //both swaps failed, try new element
                attempts++;
                searchElement = rand()%npeSize;
                tempEle = searchElement;
            }
            break;
            default:
            foundOperatorRight = true;
            foundOperatorLeft = true;
            balloting = true;
            normalized = true;
            //Check the right
            tempEle++;
            if(tempEle > npeSize -1){ // Out of bounds!
                 foundOperatorRight = false; //check left
            }
            else if(NPE[tempEle] == 'V' || NPE[tempEle] == 'H'){ //found an operator
                swap(NPE[tempEle], NPE[searchElement]);
                BallotProp(NPE, balloting);
                Nomralized(NPE, normalized);
                if(balloting && normalized){
                    //exit loop
                    attempts = 5;
                }
                else{
                 //undo swap
                 swap(NPE[tempEle], NPE[searchElement]);
                 foundOperatorRight = false;
                }
             }
             else{ //another operand, need operator, check left
                foundOperatorRight = false;
             }
            //If right faild check the left
            if(!foundOperatorRight){
            balloting = true;
            normalized = true;
            tempEle = searchElement;
            tempEle--;
            if(tempEle < 0){ // Out of bounds!
                  foundOperatorLeft = false; //exit the loop
             }
             else if(NPE[tempEle] == 'V' || NPE[tempEle] == 'H'){ //found another link left!
                swap(NPE[tempEle], NPE[searchElement]);
                BallotProp(NPE, balloting);
                Nomralized(NPE, normalized);
                if(balloting && normalized){
                    //exit loop
                    attempts = 5;
                }
                else{
                 //undo swap
                 swap(NPE[tempEle], NPE[searchElement]);
                 foundOperatorLeft = false;
                }
             }
             else{
                foundOperatorLeft = false;
             }
                
            }
            if(!foundOperatorRight && !foundOperatorLeft){ //both swaps failed, try new element
                attempts++;
                searchElement = rand()%npeSize;
                tempEle = searchElement;
            }
            break;
        }

    }while(attempts < 5);
}

/*/ BROKEN MAY FIX LATER
void optimize(vector<Node> & obj){
    vector <Node> tempVector1 = obj;
    vector <Node> tempVectorWidth;
    vector <Node> tempVectorHeight;
    vector <Node> finalVector;
    Node tempNode;
    
    double minHeight;
    double absMinHeight = 99999;
    double minWidth;
    double absMinWidth = 99999;

    int minHeightIndex;
    int minWidthIndex;

    int sizeWidth = 0;
    int sizeHeight = 0;


    int size = tempVector1.size();
    cout << "TempVector1 Size: " << size << endl;
    for(int u = 0; u < size; u++){
        if (u >= 20 ){ break;}
        cout << "TempVect1 [" << u << "] ";
        cout << "(Width: " << tempVector1[u].width << " , ";
        cout << "(Height: " <<  tempVector1[u].height << ")" << endl;
    }
    
    //Test for matching widths
    for(int i = 0; i < size; i++){
        //cout << "Comparing (width: " << tempVector1[i].width << " ,height: " << tempVector1[i].height << ")" << endl;
        for(int j = i; j < size -1; j++){
            //cout << "Comparing (width: " << tempVector1[j+1].width << " ,height: " << tempVector1[j+1].height << ")" << endl;
            //cout << "Inside j iterator which will run " << size -1 << endl;
            //cout << "tempVector1 size: " << tempVector1.size() << endl;
            //cout << "Result of " << i << "th double equals:" <<  double_equals(tempVector1[i].width, tempVector1[j+1].width, 0.001) << endl;
            if(double_equals(tempVector1[i].width, tempVector1[j+1].width, 0.001)){
                tempVectorWidth.push_back(tempVector1[j+1]);
               // cout << "tempVector1 value at pos is deleted: " << i+j+1 << endl;   
                tempVector1.erase(tempVector1.begin()+j+1);
                //sizeWidth = tempVectorWidth.size();
               // cout << "Stored (width: " << tempVectorWidth[sizeWidth - 1].width << " ,height: " << tempVectorWidth[sizeWidth - 1].height << ")" << endl;
                }
            }
            if(tempVectorWidth.size() == 0){
            //cout << "No match found" << endl;
            finalVector.push_back(tempVector1[i]);
            }
        else{
            //cout << "Match found" << endl;
            tempVectorWidth.push_back(tempVector1[i]);
            tempVector1.erase(tempVector1.begin() + i);
            i--;
            //cout << "tempVectorWidth size " << tempVectorWidth.size() << endl;
            for(int k = 0; k < tempVectorWidth.size(); k++ ){
               minHeight = tempVectorWidth[k].height;
            //   cout << "Comparing (height: " << tempVectorWidth[k].height << ")" << endl;
               if(minHeight < absMinHeight){
                   absMinHeight = minHeight;
                 //  cout << "new minimum height is: " << absMinHeight << endl;
                    minHeightIndex = k;
               }
            }
            finalVector.push_back(tempVector1[minHeightIndex]);
            //cout << "finalVector size: " << finalVector.size() << endl;
        }
        size = tempVector1.size();
    }

    if(tempVector1.size() != 0){
    //Test for matching heights
    cout << "testing for matching heights " << endl;
    for(int i = 0; i < size; i++){
        for(int j = i; j < size -1; j++){
            if(double_equals(tempVector1[i].height, tempVector1[j+1].height, 0.001)){
                tempVectorHeight.push_back(tempVector1[j+1]);
                tempVector1.erase(tempVector1.begin()+j+1);
                }
            }
            if(tempVectorHeight.size() == 0){
            finalVector.push_back(tempVector1[i]);
            }
        else{
            tempVectorHeight.push_back(tempVector1[i]);
            tempVector1.erase(tempVector1.begin() + i);
            i--;
            for(int k = 0; k < tempVectorHeight.size(); k++ ){
               minWidth = tempVectorHeight[k].width;
               if(minWidth < absMinWidth){
                   absMinWidth = minWidth;
                    minWidthIndex = k;
               }
            }
            finalVector.push_back(tempVector1[minWidthIndex]);
        }
        size = tempVector1.size();
    }

    cout << "Final Vector Size: " << finalVector.size() << endl;
    obj = finalVector;
    }
}


bool double_equals(double a, double b, double epsilon = 0.001)
{
    return std::abs(a - b) < epsilon;
}
*/

//***CONFIRMED WORKING - ONLY ISSUE IS IT SAVES LAST VECTOR TWICE
void openFile(vector<operandInfo> & obj) {
    string line;
    operandInfo operand;
    ifstream inputFile;
    int index = 0;

	inputFile.open("input.txt"); //attempt to open input file

	if (inputFile) {
		while (getline(inputFile, line)) {
            stringstream input(line);
            input >> operand.name;
            input >> operand.area;
            input >> operand.aspectRatio;
            operand.operandIndex = index++;
            //cout << "Operand: " << operand.name << " ";
            //cout << "Area: " << operand.area << " ";
            //cout << "AspectRatio: " << operand.aspectRatio << " ";
            //cout << "Index: " << operand.operandIndex << endl;
            obj.push_back(operand);
            //Saves the last line twice in the vector
		}
		inputFile.close();
	}
	else {
		cout << "Error opening file." << std::endl;
	}
}

int integerLookup(string obj1, vector < vector<Node> > & obj2){
    int value = -1;
   for(int i = 0; i < obj2.size(); i++){
       if(obj1.compare(obj2[i][0].name) == 0){return obj2[i][0].nodeIndex;}
   }
      return value;
}

//VERIFIED WORKING-----------------------
void nodeValue(vector < vector<Node> > & obj, vector<operandInfo> & obj2){
    int size = obj2.size() - 1;

    for(int i = 0; i < size; i++){
        for(int j = 0; j < 2; j++){
            if(j == 0){
            obj[i][j].name = obj2[i].name;
            obj[i][j].height = sqrt(obj2[i].area * obj2[i].aspectRatio);
            obj[i][j].width = sqrt(obj2[i].area * (1 / obj2[i].aspectRatio));
            obj[i][j].nodeIndex = obj2[i].operandIndex;
           // cout << "Node [" << i << "][" << j << "]-(Width:" << obj[i][j].width << ", ";
           // cout << "Height:" << obj[i][j].height << ", ";
           // cout << "Index: " << obj[i][j].nodeIndex << ")" << endl;
            }
            else{
            obj[i][j].name = obj2[i].name;
            obj[i][j].height = obj[i][j-1].width;
            obj[i][j].width = obj[i][j-1].height;
            obj[i][j].nodeIndex = obj2[i].operandIndex;
           // cout << "Node [" << i << "][" << j << "] ";
            //cout << "(Name: " << obj[i][j].name << ", ";
           // cout << "Width:" << obj[i][j].width << ", ";
           // cout << "Height:" << obj[i][j].height << ", ";
           // cout << "Index: " << obj[i][j].nodeIndex << ")" << endl;
            //cout << "Node [" << i << "][" << j << "]-(Width:" << obj[i][j].width << ", Height:" << obj[i][j].height << ")" << endl;
            }
        }
    }
}

double Anneal::calcCost(string NPE){
    typedef vector < vector <Node> > vertexValue;
    
    stack <string> tree;
    stack < vector <Node> > treeValues;
    vector <Node> tempVector1;
    vector <Node> tempVector2;
    vector <Node> tempVector3;
    vector <operandInfo> operands;

    string subTree;
    string middle;
    string bottom;
    string operatorHV;
    string temp;
    string NPECopy = NPE;

    Node nodeTemp;

    int size = NPE.size();
    int start = 0;
    int stop = 0;
    int operandSize = 0;
    int operatorCount = -1;
    int intBottom;
    int intMiddle;

    double area1 = 0;
    double area2 = 0;
    double minArea = 9999999;

    //read in file with string values
    openFile(operands);
    //generate matrix with each vertexValue;
    operandSize = operands.size() - 1;
    vertexValue matrix(operandSize, vector<Node>(2));
    nodeValue(matrix, operands);

    while(start != NPECopy.size()){
        //cout << "Enter While, Start: " << start << " , Stop: " << stop << endl;
       stop = NPECopy.find_first_of("VH");
       //Push elements onto the stack
       for(int i = start; i <= stop; i++){
           temp = NPECopy[i];
           //cout << "Value stored on Stack: " << temp << endl;
           tree.push(temp);
       }
       //modify NPE string
       NPECopy.erase(0,stop + 1);
       //cout << "NPECopy after Erase: " << NPECopy << endl;
       
       //remove elements off of the stack
       for(int j = 0; j < 3; j++){
           if(tree.size() == 0){
               j = 3; //exit this loop
           }
           else{
               if(j == 0){
                    operatorHV = tree.top();
                    operatorCount++;
                        }
               if(j == 1){middle = tree.top();}
               if(j == 2){bottom = tree.top();}
               subTree += tree.top();
               //cout << "Value poped off Stack: " << subTree << endl;
               tree.pop();
           }
      } 

       //is the operator H or V?
        if(operatorHV.compare("H") == 0){
           //perform operations for H
        if(middle.size() == 1){
            if(bottom.size() == 1){
                //Nothing to pop off of treeValue
                    intBottom = integerLookup(bottom, matrix);
                    intMiddle = integerLookup(middle, matrix);
                    
                    for(int j = 0; j < 2; j++){
                            nodeTemp.width = max(matrix[intBottom][j].width, matrix[intMiddle][0].width);
                            nodeTemp.height = matrix[intBottom][j].height + matrix[intMiddle][0].height;
                            tempVector1.push_back(nodeTemp);

                            nodeTemp.width = max(matrix[intBottom][j].width, matrix[intMiddle][1].width);
                            nodeTemp.height = matrix[intBottom][j].height + matrix[intMiddle][1].height;
                            tempVector1.push_back(nodeTemp);
                        }
                treeValues.push(tempVector1); //Stores our different width and height combinations
                tree.push(subTree); //Stores the letters back in main stack
                tempVector1.clear();
                //extract data from appropriate 
            }
            else{
                //Need to pop off 1 value off of treeValue
                intMiddle = integerLookup(middle, matrix);
                tempVector1 = treeValues.top(); 
                treeValues.pop();

                for(int j = 0; j < tempVector1.size(); j++){
                    nodeTemp.width = max(tempVector1[j].width, matrix[intMiddle][0].width);
                    nodeTemp.height = tempVector1[j].height + matrix[intMiddle][0].height;
                    tempVector2.push_back(nodeTemp);

                    nodeTemp.width = max(tempVector1[j].width, matrix[intMiddle][1].width);
                    nodeTemp.height = tempVector1[j].height + matrix[intMiddle][1].height;
                    tempVector2.push_back(nodeTemp);
                }
                treeValues.push(tempVector2);
                tempVector1 = treeValues.top();
                tree.push(subTree);
                tempVector1.clear();
                tempVector2.clear();
            }

        }
        else{
            if(bottom.size() == 1){
                //Need to pop off 1 value off of treeValue
                intBottom = integerLookup(bottom, matrix);
                tempVector1 = treeValues.top(); 
                treeValues.pop();

                for(int j = 0; j < tempVector1.size(); j++){
                    nodeTemp.width = max(tempVector1[j].width, matrix[intBottom][0].width);
                    nodeTemp.height = tempVector1[j].height + matrix[intBottom][0].height;
                    tempVector2.push_back(nodeTemp);

                    nodeTemp.width = max(tempVector1[j].width, matrix[intBottom][1].width);
                    nodeTemp.height = tempVector1[j].height + matrix[intBottom][1].height;
                    tempVector2.push_back(nodeTemp);
                }
                treeValues.push(tempVector2);
                tempVector1 = treeValues.top();
                tree.push(subTree);
                tempVector1.clear();
                tempVector2.clear();
            }
            else{
            //Need to pop off 2 values off of treeValue
            tempVector1 = treeValues.top();
            treeValues.pop();
            tempVector2 = treeValues.top();
            treeValues.pop();

            for(int i = 0; i < tempVector1.size(); i++){
                for(int j = 0; j < tempVector2.size(); j++){
                    nodeTemp.width = max(tempVector1[i].width, tempVector2[j].width);
                    nodeTemp.height = tempVector1[i].height + tempVector2[j].height;
                    tempVector3.push_back(nodeTemp);
                }
            }
            treeValues.push(tempVector3);
            tree.push(subTree);
            tempVector1.clear();
            tempVector2.clear();
            tempVector3.clear();
        }
        }
       }
       else{
           //perform operations for V
        if(middle.size() == 1){
            if(bottom.size() == 1){
                //Nothing to pop off of treeValue
                    intBottom = integerLookup(bottom, matrix);
                    intMiddle = integerLookup(middle, matrix);
                    
                    for(int j = 0; j < 2; j++){
                            nodeTemp.height = max(matrix[intBottom][j].height, matrix[intMiddle][0].height);
                            nodeTemp.width = matrix[intBottom][j].width + matrix[intMiddle][0].width;
                            tempVector1.push_back(nodeTemp);

                            nodeTemp.height = max(matrix[intBottom][j].height, matrix[intMiddle][1].height);
                            nodeTemp.width = matrix[intBottom][j].width + matrix[intMiddle][1].width;
                            tempVector1.push_back(nodeTemp);
                        }
                treeValues.push(tempVector1); //Stores our different width and height combinations
                tree.push(subTree); //Stores the letters back in main stack
                tempVector1.clear();
            }
            else{
                //Need to pop off 1 value off of treeValue
                intMiddle = integerLookup(middle, matrix);
                tempVector1 = treeValues.top();
                treeValues.pop();

                for(int j = 0; j < tempVector1.size(); j++){
                    nodeTemp.height = max(tempVector1[j].height, matrix[intMiddle][0].height);
                    nodeTemp.width = tempVector1[j].width + matrix[intMiddle][0].width;
                    tempVector2.push_back(nodeTemp);

                    nodeTemp.height = max(tempVector1[j].height, matrix[intMiddle][1].height);
                    nodeTemp.width = tempVector1[j].width + matrix[intMiddle][1].width;
                    tempVector2.push_back(nodeTemp);
                }
                treeValues.push(tempVector2);
                tree.push(subTree);
                tempVector1.clear();
                tempVector2.clear();
            }
        }
        else{
            if(bottom.size() == 1){
            //Need to pop off 1 value off of treeValue
            intBottom = integerLookup(bottom, matrix);
            tempVector1 = treeValues.top();
            treeValues.pop();

            for(int j = 0; j < tempVector1.size(); j++){
                nodeTemp.height = max(tempVector1[j].height, matrix[intBottom][0].height);
                nodeTemp.width = tempVector1[j].width + matrix[intBottom][0].width;
                tempVector2.push_back(nodeTemp);

                nodeTemp.height = max(tempVector1[j].height, matrix[intBottom][1].height);
                nodeTemp.width = tempVector1[j].width + matrix[intBottom][1].width;
                tempVector2.push_back(nodeTemp);
            }
            treeValues.push(tempVector2);
            tree.push(subTree);
            tempVector1.clear();
            tempVector2.clear();
            }
            else{
            //Need to pop off 2 values off of treeValue
            tempVector1 = treeValues.top();
            treeValues.pop();
            tempVector2 = treeValues.top();
            treeValues.pop();

            for(int i = 0; i < tempVector1.size(); i++){
                for(int j = 0; j < tempVector2.size(); j++){
                    nodeTemp.height = max(tempVector1[i].height, tempVector2[j].height);
                    nodeTemp.width = tempVector1[i].width + tempVector2[j].width;
                    tempVector3.push_back(nodeTemp);
                }
            }
            treeValues.push(tempVector3);
            tree.push(subTree);
            tempVector1.clear();
            tempVector2.clear();
            tempVector3.clear();
        }
        }
       }
       subTree = "";
     } //end of while loop

     //pop off the last vector, values at the top with the root
     //calculate area and find the minimum
    tempVector3 = treeValues.top();
    for(int i = 0; i < tempVector3.size(); i++){
        area1 = tempVector3[i].width * tempVector3[i].height;
        if(area1 < minArea){
            minArea = area1;
        }  
    }

    //return the minimum area
    return minArea;
}

void Anneal::runAlgorithm(){
    double deltaCost = 0;
    double e = 2.71828;
    double RANDOM = 0.0;
    string newE = E0;
    double power = 0;
    
    do{
        MT = uphill = Reject = 0;
        do{ 
            moveSelect(newE);  //Select a random move and apply it to E
            MT+= 1;         // add 1 to moves at this temperature
            deltaCost = calcCost(newE) - calcCost(NPE);
            cout << "deltaCost = " << deltaCost << endl;
            RANDOM = (double)rand()/((double)RAND_MAX+1); //value between 0 and 1
            cout << "RANDOM: " << RANDOM << endl;
            cout << "Pow: " << pow(e,(-deltaCost/T)) << endl;
            if(deltaCost < 0 || RANDOM < pow(e,(-deltaCost/T))){
                //if the cost went down, or we satisfy the random function
                if(deltaCost > 0){ uphill++;}
                else{
                    NPE = newE; // accept the new E
                    if(calcCost(NPE) < calcCost(Best)){Best = NPE;
                    cout << "Best: " << Best << endl;
                    } //Update best if applicable
                }
            }
            else{
                Reject++;
            }
            cout << "uphill: " << uphill << endl;
         } while((uphill < N) && (MT < 2*N));
         cout << "uphill: " << uphill << endl;
         cout << "Reject: " << Reject << endl;
         cout << "MT: " << MT << endl;
         if(T < lambdatf * T0){
             ratio = 0.1;
         }
         T = T*ratio;
         cout << "T: " << T << endl;
         cout << "Reject/MT: " << Reject/MT << endl;
    } while(((Reject/MT) < 0.95) && (T >= epsilon) ); // May add Out of TIME
}