

if (Meteor.isClient){

Template.switchOff.events({


    'click': function(){
    	event.preventDefault();
        lastValue=States.findOne({id:this.switch_id},{sort:{createdAt:-1},limit:1}).lastValueSent;
        //currentState=States.findOne({id:this.switch_id},{sort:{createdAt:-1},limit:1}).state;
        //console.log(value);
        initial_state=Events.findOne({switch_id:this.switch_id},{sort:{updated_at:-1},limit:1}).init_state;
      //at Arduino boot
        if(initial_state=="1"){
          States.insert({
            id:this.switch_id,
            state:"0",
            createdAt: new Date(),
            type:"switch",
            lastValueSent:"1"


        });
        } 
        //during normal operation after the first turn off
        if(initial_state=="0" && lastValue==="1" && this.switch_state==1){
          console.log("Inserting 0");
           
          States.insert({
          id:this.switch_id,
          state:"0",
          //createdAt: new Date(),
          type:"switch",
          lastValueSent:"1",
          sentFromWeb:"Yesl_lv_1"

         
        });
        }else if(initial_state=="0" && lastValue==="0" && this.switch_state==1){
          console.log("Inserting 1");
           
          States.insert({
          id:this.switch_id,
          state:"1",
          //createdAt: new Date(),
          type:"switch",
          lastValueSent:"1",
          sentFromWeb:"Yes_lv_0"
        

      });
      }
      //Meteor.call('runCode',0, function (err, response) {
  		//console.log(response);
  		//console.log("responded");
//});
   }
});
}

Template.switchOff.helpers({
   showTheButton:function(){
      if(this.switch_state==1){
        return true;

    }else if (this.switch_state==0)
    {
      return false;
    }
  }
});