Template.switchOff.events({
    'click': function(){
    	event.preventDefault();
        lastValue=States.findOne({id:this.switch_id},{sort:{createdAt:-1},limit:1}).lastValueSent;
        //currentState=States.findOne({id:this.switch_id},{sort:{createdAt:-1},limit:1}).state;
        //console.log(value);
      
        if(lastValue==="1" && this.switch_state==1){
          //console.log(this.switch_id);
          States.insert({
          id:this.switch_id,
          state:"0",
          createdAt: new Date(),
          type:"switch",
          lastValueSent:"1"
        })
        }else if(lastValue==="0" && this.switch_state==1){
          States.insert({
          id:this.switch_id,
          state:"1",
          createdAt: new Date(),
          type:"switch",
          lastValueSent:"1"

      });
      }
      //Meteor.call('runCode',0, function (err, response) {
  		//console.log(response);
  		//console.log("responded");
//});
    }
});

Template.switchOff1.events({
    'click': function(){
      event.preventDefault();
      console.log(this.switch_id);
      States.insert({
          id:this.switch_id,
          state:"0",
          createdAt: new Date(),
          type:"switch"
    });
      //Meteor.call('runCode',0, function (err, response) {
      //console.log(response);
      //console.log("responded");
//});
    }
});

Template.switchOff2.events({
    'click': function(){
    	event.preventDefault();
      States.insert({
        id:this.switch_id,
        state:"1",
        createdAt: new Date(),
        type:"switch"
      });

        console.log("sending 1");
        //Meteor.call('runCode',1, function (err, response) {
  		  //console.log(response);
  		  //console.log("responded");
//});
    }
});