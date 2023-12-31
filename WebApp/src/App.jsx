import { useEffect, useState } from 'react'
import './App.css'
import TopBar from './Components/Topbar'
import Login from './Components/Login'
import SignUp from './Components/SignUp'
import Routs from './Routs'

function App() {
  const [isAuth, setIsAuth] = useState(false)
  const [isSign, isSignUp] = useState(false)
  useEffect(() => {
    if (localStorage.getItem('user')) {
      const item = localStorage.getItem('user')
      const data = JSON.parse(item)
      setIsAuth(data.isAuthenticated)
    }
  }, [])
  return (
    <>
      {isAuth ? (
        <>
          <TopBar />
          <Routs />
        </>
      ) : isSign ? (
        <SignUp isSignUp={isSignUp} />
      ) : (
        <Login isSignUp={isSignUp} />
      )}
    </>
  );
}
export default App
