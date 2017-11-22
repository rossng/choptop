<template>
  <div id="app">
    <NavButton direction="up" :pressed="latestUpdate.upPressed"/>
    <NavButton direction="left" :pressed="latestUpdate.leftPressed"/>
    <div id="main">
      <WelcomeToChopTop/>
    </div>
    <NavButton direction="right" :pressed="latestUpdate.rightPressed"/>
    <NavButton direction="down" :pressed="latestUpdate.downPressed"/>
    <button v-on:click="leftPressed = !leftPressed">toggleLeft</button>
  </div>

</template>

<script>
import WelcomeToChopTop from './components/WelcomeToChopTop'
import NavButton from './components/NavButton'

export default {
  name: 'app',
  components: {
    WelcomeToChopTop,
    NavButton
  },

  data (){
    return {
      //button presses
      // leftPressed : true,
      // rightPressed : false,
      // upPressed : false,
      // downPressed: false,


      //data fetching from server
      error: null,
      latestUpdate: {}



    }
  },

  created () {
    // fetch the data when the view is created and the data is
    // already being observed
    this.fetchData()
  },


  methods: {
    fetchData () {
      this.error = this.update = null
      this.loading = true
      // replace `getupdate` with your data fetching util / API wrapper
      // $.get("http://localhost:8432", (update, err) => {
      //   this.loading = false
      //   if (err.toString() != "success") {
      //     this.error = err.toString()
      //   } else {
      //     this.latestUpdate = update
      //   }
      // })
    }
  }




}
</script>

<style>
#app {
  font-family: 'Avenir', Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  height:100%;
  width:100%;
  /*overflow:hidden;*/
}




#main{
  width:760px;
  height:460px;
  float:left;
  display: inline-block;
}
</style>
