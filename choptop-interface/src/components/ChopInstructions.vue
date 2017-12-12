<template>
  <div class="toChop">
    Ingreeedient
  </div>


  
</template>

<script>


  export default {
    name: 'chopInstructions',
    components: {
    },

    props: ['steps', 'stepIdx',],
    data () {
      return {
        selected: true,
        timeDisp:false,
      }
    },

    methods: {
        
    }
  }
</script>



<style scoped>
  .step{
      font-size: 30px;
      margin:30px;
  }
  
</style>