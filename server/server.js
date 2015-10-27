if(Meteor.isServer){
//Meteor.startup(function () {


  Meteor.methods({

    runCode: function (state) {
      // This method call won't return immediately, it will wait for the
      // asynchronous code to finish, so we call unblock to allow this client
      // to queue other method calls (see Meteor docs)
      this.unblock();
            // load exec
      var exec=Npm.require("child_process").exec;
// load future from fibers
     // var Future=Npm.require("fibers/future");
     // var future=new Future();
      if (state == 0){
       var command="echo 0 > /home/pi/switch/command";   
      }

      if (state == 1){
        var command="echo 1 > /home/pi/switch/command";
      }
      
      exec(command,function(error,stdout,stderr){
          if(error){
              console.log(error);
              throw new Meteor.Error(500,command+" failed");
          }else{
            console.log(stdout.toString());
            return(stdout.toString());
          }

         
      });
     // return future.wait();
    }
   
  });
}
//});