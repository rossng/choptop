<template>
  <div class="toChop">
    <img :src="getimage(extra)"/>
  </div>


  
</template>

<script>


  export default {
    name: 'chopInstructions',
    components: {
    },

    props: ['extra'],
    data () {
      return {
      }
    },

    methods: {
      getimage(extra){
        console.log(extra);
        switch(extra){
          case "slice onion":
            return "/src/assets/instructions/sliceOnion.gif";
          case "slice tomato":
            return "/src/assets/instructions/sliceTomato.gif";
          case "slice pepper":
            return "/src/assets/instructions/slicePepper.gif";
          case "chicken strips":
            return "/src/assets/instructions/chickenStrips.gif";
          case "chicken strips":
            return "/src/assets/instructions/chickenStrips.gif";
          case "slice spring onions":
            return "/src/assets/instructions/sliceSpringOnions.gif";
          case "broccoli":
            return "/src/assets/instructions/cutBroccoli.gif";
          case "slice mushrooms":
            return "/src/assets/instructions/sliceMushrooms.gif";
          case "slice cucumber":
            return "/src/assets/instructions/sliceCucumber.gif";
        }
      },
    }
  }
</script>



<style scoped>
  .toChop{
    width:100%;
    display: flex;
    align-items: center;
  }
</style>