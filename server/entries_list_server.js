/*Template.eventsList.helpers({

  var pipeline =
      [
        {$sort: {updated_at: -1}},
        {
         $group:
         {
           lastEntry:{$last:"$updated_at"}
         }
        }
      ];
return result =  Events.aggregate(pipeline);
});
*/