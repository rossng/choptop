<template>
    <img :src="getImage(extra)" class="toChop"/>
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
      getImage(extra){
        console.log(extra);
        switch(extra){
          case "slice onion":
            return "/src/assets/instructions/sliceOnion.gif";
          case "slice tomato":
            return "/src/assets/instructions/sliceTomato.gif";
          case "slice pepper":
            return "/src/assets/instructions/slicePepper.gif";
          case "chicken strips":
            return "/src/assets/instructions/chickenStrips.jpg";
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
  .toChop {
    max-width: 600px;
    max-height: 300px;
    display: flex;
    align-items: center;
    margin: 0 auto;
  }
</style>
