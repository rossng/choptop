<template>
	<div class="verticalCenter welcome">
		<div>
			<h1>Welcome to Choptop</h1>
			<h2>Press at the bottom to start</h2>
			<icon name="angle-down" scale="4"></icon>
		</div>
	</div>

</template>

<script>
	export default {
	  name: 'WelcomeToChopTop',
	  data () {
	    return {
	      
	    }
	  }
	}
</script>



<style scoped>
	h1, h2 {
	  font-weight: normal;
	  text-align: center;
	}

	.welcome{
		padding-top:60px;
		width:760px;
	}
	
</style>
